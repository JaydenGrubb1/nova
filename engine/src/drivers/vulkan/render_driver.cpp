/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifdef NOVA_VULKAN

#include "drivers/vulkan/render_driver.h"

#include <nova/core/debug.h>
#include <nova/platform/window_driver.h>
#include <nova/render/render_device.h>
#include <nova/version.h>
#include <vulkan/vulkan.h>

#include <format>
#include <unordered_map>

#define VALIDATION_LAYER "VK_LAYER_KHRONOS_validation"

using namespace Nova;

VulkanRenderDriver::VulkanRenderDriver(WindowDriver* p_driver) : m_window_driver(p_driver) {
	NOVA_AUTO_TRACE();
	_check_version();
	_check_extensions();
	_check_layers();
	_init_instance();
	_init_hardware();
}

VulkanRenderDriver::~VulkanRenderDriver() {
	NOVA_AUTO_TRACE();
	if (m_device) {
		vkDestroyDevice(m_device, get_allocator(VK_OBJECT_TYPE_DEVICE));
	}
	if (m_instance) {
		vkDestroyInstance(m_instance, get_allocator(VK_OBJECT_TYPE_INSTANCE));
	}
}

RenderAPI VulkanRenderDriver::get_api() const {
	return RenderAPI::VULKAN;
}

u32 VulkanRenderDriver::get_api_version() const {
	u32 version;
	vkEnumerateInstanceVersion(&version);
	return version;
}

std::string VulkanRenderDriver::get_api_name() const {
	return "Vulkan";
}

std::string VulkanRenderDriver::get_api_version_string() const {
	const u32 version = get_api_version();
	return std::format(
		"{}.{}.{}-{}",
		VK_API_VERSION_MAJOR(version),
		VK_API_VERSION_MINOR(version),
		VK_API_VERSION_PATCH(version),
		VK_API_VERSION_VARIANT(version)
	);
}

u32 VulkanRenderDriver::get_device_count() const {
	return static_cast<u32>(m_devices.size());
}

const RenderDevice& VulkanRenderDriver::get_device(const u32 p_index) const {
	NOVA_ASSERT(p_index < m_devices.size());
	return m_devices[p_index];
}

bool VulkanRenderDriver::get_device_supports_surface(const u32 p_index, SurfaceID p_surface) const {
	NOVA_AUTO_TRACE();
	NOVA_ASSERT(p_index < m_devices.size());
	NOVA_ASSERT(p_surface);

	// TODO: Check other queue families?

	VkBool32 supported = false;
	if (vkGetPhysicalDeviceSurfaceSupportKHR(
			static_cast<VkPhysicalDevice>(m_devices[p_index].handle),
			0,
			p_surface->handle,
			&supported
		)
		!= VK_SUCCESS) {
		return false;
	}
	return supported;
}

void VulkanRenderDriver::select_device(const u32 p_index) {
	NOVA_AUTO_TRACE();
	NOVA_ASSERT(!m_device);
	NOVA_ASSERT(p_index < m_devices.size());

	NOVA_LOG("Using device: {}", m_devices[p_index].name);
	m_physical_device = static_cast<VkPhysicalDevice>(m_devices[p_index].handle);

	_check_device_extensions();
	_check_device_features();
	_check_device_capabilities();

	std::vector<VkDeviceQueueCreateInfo> queues;
	_init_queues(queues);
	_init_device(queues);
}

SurfaceID VulkanRenderDriver::create_surface(WindowID p_window) {
	NOVA_AUTO_TRACE();
	NOVA_ASSERT(m_window_driver);
	return m_window_driver->create_surface(p_window, this);
}

void VulkanRenderDriver::destroy_surface(SurfaceID p_surface) {
	NOVA_AUTO_TRACE();
	vkDestroySurfaceKHR(m_instance, p_surface->handle, get_allocator(VK_OBJECT_TYPE_SURFACE_KHR));
	delete p_surface;
}

VkInstance VulkanRenderDriver::get_instance() const {
	return m_instance;
}

VkAllocationCallbacks* VulkanRenderDriver::get_allocator(const VkObjectType p_type) const {
	// TODO: Add custom allocator
	(void)p_type;
	return nullptr;
}

void VulkanRenderDriver::_check_version() const {
	NOVA_AUTO_TRACE();

	const u32 version = get_api_version();
	if (version < VK_API_VERSION_1_2) {
		throw std::runtime_error("Vulkan API version is too low");
	}

	NOVA_LOG(
		"Vulkan API version: {}.{}.{}-{}",
		VK_API_VERSION_MAJOR(version),
		VK_API_VERSION_MINOR(version),
		VK_API_VERSION_PATCH(version),
		VK_API_VERSION_VARIANT(version)
	);
}

void VulkanRenderDriver::_check_extensions() {
	NOVA_AUTO_TRACE();

	u32 count;
	std::unordered_map<std::string_view, bool> requested; // <extension, required>

	const auto surface_extension = m_window_driver->get_surface_extension();
	if (!surface_extension) {
		throw std::runtime_error("Could not determine required surface extension");
	}

	requested[VK_KHR_SURFACE_EXTENSION_NAME] = true;
	requested[surface_extension] = true;

	// Add optional extensions
	requested[VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME] = false;
	if (Debug::is_debug()) {
		requested[VK_EXT_DEBUG_REPORT_EXTENSION_NAME] = false;
		requested[VK_EXT_DEBUG_UTILS_EXTENSION_NAME] = false;
	}

	// Get available extensions
	vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr); // TODO: Check result
	std::vector<VkExtensionProperties> available(count);
	vkEnumerateInstanceExtensionProperties(nullptr, &count, available.data()); // TODO: Check result

	// Check found extensions
	for (const auto& extension : available) {
		if (auto it = requested.find(extension.extensionName); it != requested.end()) {
			NOVA_LOG("Using extension: {}", extension.extensionName);
			m_extensions.push_back(it->first.data());
			requested.erase(it);
		}
	}

	// Check remaining extensions
	bool error = false;
	for (auto [name, required] : requested) {
		if (required) {
			NOVA_ERROR("Required extension not found: {}", name);
			error = true;
		} else {
			NOVA_WARN("Optional extension not found: {}", name);
		}
	}
	if (error) {
		throw std::runtime_error("Failed to find required extensions");
	}
}

void VulkanRenderDriver::_check_layers() {
	NOVA_AUTO_TRACE();

	// Skip if not in debug mode
	if (!Debug::is_debug()) {
		return;
	}

	// Get available layers
	u32 count;
	vkEnumerateInstanceLayerProperties(&count, nullptr);
	std::vector<VkLayerProperties> available(count);
	vkEnumerateInstanceLayerProperties(&count, available.data());

	// Check found layers
	for (const auto& layer : available) {
		if (std::string_view(layer.layerName) == VALIDATION_LAYER) {
			NOVA_LOG("Using layer: {}", layer.layerName);
			m_layers.push_back(VALIDATION_LAYER);
			return;
		}
	}

	NOVA_WARN("Validation layer not found");
}

void VulkanRenderDriver::_init_instance() {
	NOVA_AUTO_TRACE();

	VkApplicationInfo appinfo {};
	appinfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appinfo.pApplicationName = "App"; // TODO: Get from config
	appinfo.applicationVersion = 0; // TODO: Get from config
	appinfo.pEngineName = "Nova";
	appinfo.engineVersion = VK_MAKE_API_VERSION(0, NOVA_VERSION_MAJOR, NOVA_VERSION_MINOR, NOVA_VERSION_PATCH);
	appinfo.apiVersion = VK_API_VERSION_1_2;

	VkInstanceCreateInfo create {};
	create.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	create.pApplicationInfo = &appinfo;
	create.enabledLayerCount = static_cast<u32>(m_layers.size());
	create.ppEnabledLayerNames = m_layers.data();
	create.enabledExtensionCount = static_cast<u32>(m_extensions.size());
	create.ppEnabledExtensionNames = m_extensions.data();

	if (vkCreateInstance(&create, get_allocator(VK_OBJECT_TYPE_INSTANCE), &m_instance) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create VkInstance");
	}

	NOVA_LOG("VkInstance created");
}

void VulkanRenderDriver::_init_hardware() {
	NOVA_AUTO_TRACE();

	u32 count;
	vkEnumeratePhysicalDevices(m_instance, &count, nullptr); // TODO: Check result
	std::vector<VkPhysicalDevice> devices(count);
	vkEnumeratePhysicalDevices(m_instance, &count, devices.data()); // TODO: Check result

	m_devices.reserve(count);

	for (const auto& device : devices) {
		VkPhysicalDeviceProperties properties;
		vkGetPhysicalDeviceProperties(device, &properties);

		m_devices.emplace_back();
		m_devices.back().name = properties.deviceName;
		m_devices.back().vendor = static_cast<RenderDevice::Vendor>(properties.vendorID);
		m_devices.back().type = static_cast<RenderDevice::Type>(properties.deviceType);
		m_devices.back().deviceID = properties.deviceID;
		m_devices.back().handle = device;

		NOVA_LOG("Found device: {}", properties.deviceName);
	}

	if (m_devices.empty()) {
		throw std::runtime_error("No devices found");
	}
}

void VulkanRenderDriver::_check_device_extensions() {
	NOVA_AUTO_TRACE();

	std::unordered_map<std::string_view, bool> requested; // <extension, required>
	requested[VK_KHR_SWAPCHAIN_EXTENSION_NAME] = true;
	// TODO: Add other device extensions

	// Get available extensions
	u32 count;
	vkEnumerateDeviceExtensionProperties(m_physical_device, nullptr, &count, nullptr); // TODO: Check result
	std::vector<VkExtensionProperties> available(count);
	vkEnumerateDeviceExtensionProperties(m_physical_device, nullptr, &count, available.data()); // TODO: Check result

	// Check found extensions
	for (const auto& extension : available) {
		if (auto it = requested.find(extension.extensionName); it != requested.end()) {
			NOVA_LOG("Using device extension: {}", extension.extensionName);
			m_device_extensions.push_back(it->first.data());
			requested.erase(it);
		}
	}

	// Check remaining extensions
	bool error = false;
	for (auto [name, required] : requested) {
		if (required) {
			NOVA_ERROR("Required device extension not found: {}", name);
			error = true;
		} else {
			NOVA_WARN("Optional device extension not found: {}", name);
		}
	}
	if (error) {
		throw std::runtime_error("Failed to find required device extensions");
	}
}

void VulkanRenderDriver::_check_device_features() {
	NOVA_AUTO_TRACE();

	vkGetPhysicalDeviceFeatures(m_physical_device, &m_features);
	// TODO: Check required features
	// TODO: Disable unwanted features

	NOVA_WARN("{}() not implemented", NOVA_FUNC_NAME);
}

void VulkanRenderDriver::_check_device_capabilities() {
	NOVA_AUTO_TRACE();
	NOVA_WARN("{}() not implemented", NOVA_FUNC_NAME);
	// TODO: Check device capabilities
}

void VulkanRenderDriver::_init_queues(std::vector<VkDeviceQueueCreateInfo>& p_queues) const {
	NOVA_AUTO_TRACE();

	u32 count;
	vkGetPhysicalDeviceQueueFamilyProperties(m_physical_device, &count, nullptr);
	std::vector<VkQueueFamilyProperties> available(count);
	vkGetPhysicalDeviceQueueFamilyProperties(m_physical_device, &count, available.data());

	constexpr VkQueueFlags QUEUE_MASK = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT;
	static float s_priority = 1.0f;
	VkQueueFlags found = 0;

	for (u32 i = 0; i < count; i++) {
		if ((available[i].queueFlags & QUEUE_MASK) == 0) {
			continue;
		}
		if (!available[i].queueCount) {
			continue;
		}

		NOVA_LOG("Using queue family: {}", i);
		found |= available[i].queueFlags;

		VkDeviceQueueCreateInfo queue {};
		queue.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queue.queueFamilyIndex = i;
		queue.queueCount = 1;
		queue.pQueuePriorities = &s_priority;

		p_queues.push_back(queue);
	}

	if ((found & QUEUE_MASK) != QUEUE_MASK) {
		throw std::runtime_error("Failed to find all required queue families");
	}
}

void VulkanRenderDriver::_init_device(const std::vector<VkDeviceQueueCreateInfo>& p_queues) {
	NOVA_AUTO_TRACE();

	VkDeviceCreateInfo create {};
	create.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	create.enabledLayerCount = static_cast<u32>(m_layers.size());
	create.ppEnabledLayerNames = m_layers.data();
	create.enabledExtensionCount = static_cast<u32>(m_device_extensions.size());
	create.ppEnabledExtensionNames = m_device_extensions.data();
	create.queueCreateInfoCount = static_cast<u32>(p_queues.size());
	create.pQueueCreateInfos = p_queues.data();
	create.pEnabledFeatures = &m_features;
	// TODO: pNext for additional features

	if (vkCreateDevice(m_physical_device, &create, get_allocator(VK_OBJECT_TYPE_DEVICE), &m_device) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create VkDevice");
	}

	NOVA_LOG("VkDevice created");
}

#endif // NOVA_VULKAN
