/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "drivers/vulkan/render_driver.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <nova/core/debug.h>
#include <nova/version.h>
#include <vulkan/vulkan.h>

#include <unordered_map>
#include <unordered_set>

#define VALIDATION_LAYER "VK_LAYER_KHRONOS_validation"

using namespace Nova;

VulkanRenderDriver::VulkanRenderDriver() {
	NOVA_AUTO_TRACE();
	_check_version();
	_check_extensions();
	_check_layers();
	_init_instance();
	_init_devices();
}

VulkanRenderDriver::~VulkanRenderDriver() {
	NOVA_AUTO_TRACE();
	vkDestroyInstance(m_instance, _get_allocator(VK_OBJECT_TYPE_INSTANCE));
}

RenderAPI VulkanRenderDriver::get_api() const {
	return RenderAPI::VULKAN;
}

u32 VulkanRenderDriver::get_api_version() const {
	u32 version;
	vkEnumerateInstanceVersion(&version);
	return version;
}

u32 VulkanRenderDriver::get_device_count() const {
	return m_devices.size();
}

const RenderDevice& VulkanRenderDriver::get_device(const u32 index) const {
	return m_devices[index];
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

	// Find required extensions
	SDL_Init(SDL_INIT_VIDEO); // TODO: Move to SDL driver
	const auto required = SDL_Vulkan_GetInstanceExtensions(&count);
	for (u32 i = 0; i < count; ++i) {
		requested[required[i]] = true;
	}

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
	create.enabledLayerCount = m_layers.size();
	create.ppEnabledLayerNames = m_layers.data();
	create.enabledExtensionCount = m_extensions.size();
	create.ppEnabledExtensionNames = m_extensions.data();

	if (vkCreateInstance(&create, _get_allocator(VK_OBJECT_TYPE_INSTANCE), &m_instance) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create VkInstance");
	}

	NOVA_LOG("VkInstance created");
}

void VulkanRenderDriver::_init_devices() {
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
	}
}

VkAllocationCallbacks* VulkanRenderDriver::_get_allocator(const VkObjectType type) {
	// TODO: Add custom allocator
	(void)type;
	return nullptr;
}