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
#include <nova/render/render_params.h>
#include <nova/version.h>
#include <vulkan/vulkan.h>

#include <algorithm>
#include <format>
#include <limits>
#include <unordered_map>

#define VALIDATION_LAYER "VK_LAYER_KHRONOS_validation"

using namespace Nova;

// clang-format off

static constexpr VkShaderStageFlagBits VK_SHADER_STAGE_MAP[] = {
	VK_SHADER_STAGE_VERTEX_BIT,
	VK_SHADER_STAGE_FRAGMENT_BIT,
	VK_SHADER_STAGE_GEOMETRY_BIT,
	VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT,
	VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT,
	VK_SHADER_STAGE_COMPUTE_BIT,
	VK_SHADER_STAGE_MESH_BIT_EXT,
	VK_SHADER_STAGE_TASK_BIT_EXT
};

static constexpr VkPrimitiveTopology VK_PRIMITIVE_TOPOLOGY_MAP[] = {
	VK_PRIMITIVE_TOPOLOGY_POINT_LIST,
	VK_PRIMITIVE_TOPOLOGY_LINE_LIST,
	VK_PRIMITIVE_TOPOLOGY_LINE_STRIP,
	VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
	VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP
};

static constexpr VkCullModeFlags VK_CULL_MODE_MAP[] = {
	VK_CULL_MODE_NONE,
	VK_CULL_MODE_FRONT_BIT,
	VK_CULL_MODE_BACK_BIT
};

static constexpr VkFrontFace VK_FRONT_FACE_MAP[] = {
	VK_FRONT_FACE_COUNTER_CLOCKWISE,
	VK_FRONT_FACE_CLOCKWISE
};

// clang-format on

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
	NOVA_ASSERT(p_surface);
	vkDestroySurfaceKHR(m_instance, p_surface->handle, get_allocator(VK_OBJECT_TYPE_SURFACE_KHR));
	delete p_surface;
}

SwapchainID VulkanRenderDriver::create_swapchain(SurfaceID p_surface) {
	NOVA_AUTO_TRACE();
	NOVA_ASSERT(p_surface);

	Swapchain* swapchain = new Swapchain();
	swapchain->surface = p_surface;

	u32 count;
	vkGetPhysicalDeviceSurfaceFormatsKHR(m_physical_device, p_surface->handle, &count, nullptr); // TODO: Check result
	std::vector<VkSurfaceFormatKHR> formats(count);
	vkGetPhysicalDeviceSurfaceFormatsKHR(m_physical_device, p_surface->handle, &count, formats.data()); // TODO: Check result

	const VkFormat preferred_format = VK_FORMAT_B8G8R8A8_UNORM; // TODO: Get from config?
	const VkFormat fallback_format = VK_FORMAT_R8G8B8A8_UNORM; // TODO: Get from config?

	if (count == 1 && formats[0].format == VK_FORMAT_UNDEFINED) {
		swapchain->format = preferred_format;
		swapchain->color_space = formats[0].colorSpace;
	} else {
		for (const auto& format : formats) {
			if (format.format == preferred_format || format.format == fallback_format) {
				swapchain->format = format.format;
				swapchain->color_space = format.colorSpace;
				if (swapchain->format == preferred_format) {
					break;
				}
			}
		}
	}

	if (swapchain->format == VK_FORMAT_UNDEFINED) {
		throw std::runtime_error("Failed to find a supported swapchain format");
	}

	// TODO: Create render pass

	return swapchain;
}

void VulkanRenderDriver::resize_swapchain(SwapchainID p_swapchain) {
	NOVA_AUTO_TRACE();
	NOVA_ASSERT(p_swapchain);

	Surface* surface = p_swapchain->surface;

	// TODO: Release old swapchain resources

	VkSurfaceCapabilitiesKHR capabilities;
	if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_physical_device, surface->handle, &capabilities) != VK_SUCCESS) {
		throw std::runtime_error("Failed to get surface capabilities");
	}

	VkExtent2D extent;
	if (capabilities.currentExtent.width == std::numeric_limits<u32>::max()) {
		extent.width = std::clamp(surface->width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		extent.height =
			std::clamp(surface->height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
	} else {
		extent = capabilities.currentExtent;
		surface->width = extent.width;
		surface->height = extent.height;
	}

	if (extent.width == 0 || extent.height == 0) {
		return;
	}

	u32 image_count = capabilities.minImageCount + 1;
	if (capabilities.maxImageCount > 0 && image_count > capabilities.maxImageCount) {
		image_count = capabilities.maxImageCount;
	}

	u32 present_mode_count;
	vkGetPhysicalDeviceSurfacePresentModesKHR(
		m_physical_device,
		surface->handle,
		&present_mode_count,
		nullptr
	); // TODO: Check result
	std::vector<VkPresentModeKHR> present_modes(present_mode_count);
	vkGetPhysicalDeviceSurfacePresentModesKHR(
		m_physical_device,
		surface->handle,
		&present_mode_count,
		present_modes.data()
	); // TODO: Check result

	VkPresentModeKHR present_mode = VK_PRESENT_MODE_MAILBOX_KHR; // TODO: Get from config
	if (std::find(present_modes.begin(), present_modes.end(), present_mode) == present_modes.end()) {
		NOVA_WARN("Preferred present mode not supported, falling back to FIFO");
		present_mode = VK_PRESENT_MODE_FIFO_KHR;
		// TODO: Update config
	}

	VkSwapchainCreateInfoKHR swap_create {};
	swap_create.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swap_create.surface = surface->handle;
	swap_create.minImageCount = image_count;
	swap_create.imageFormat = p_swapchain->format;
	swap_create.imageColorSpace = p_swapchain->color_space;
	swap_create.imageExtent = extent;
	swap_create.imageArrayLayers = 1; // TODO: Support VR
	swap_create.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; // TRANSFER_DST_BIT ???
	swap_create.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	swap_create.preTransform = capabilities.currentTransform;
	swap_create.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; // TODO: Support transparent windows
	swap_create.presentMode = present_mode;
	swap_create.clipped = VK_TRUE;
	swap_create.oldSwapchain = VK_NULL_HANDLE; // TODO: Handle old swapchain

	if (vkCreateSwapchainKHR(m_device, &swap_create, get_allocator(VK_OBJECT_TYPE_SWAPCHAIN_KHR), &p_swapchain->handle)
		!= VK_SUCCESS) {
		throw std::runtime_error("Failed to create swapchain");
	}

	NOVA_LOG("VkSwapchainKHR created");

	vkGetSwapchainImagesKHR(m_device, p_swapchain->handle, &image_count, nullptr); // TODO: Check result
	p_swapchain->images.resize(image_count);
	vkGetSwapchainImagesKHR(m_device, p_swapchain->handle, &image_count, p_swapchain->images.data()); // TODO: Check result

	VkImageViewCreateInfo view_create {};
	view_create.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	view_create.viewType = VK_IMAGE_VIEW_TYPE_2D;
	view_create.format = p_swapchain->format;
	view_create.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	view_create.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	view_create.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	view_create.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	view_create.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	view_create.subresourceRange.baseMipLevel = 0;
	view_create.subresourceRange.levelCount = 1;
	view_create.subresourceRange.baseArrayLayer = 0; // TODO: Support VR
	view_create.subresourceRange.layerCount = 1; // TODO: Support VR

	p_swapchain->image_views.resize(image_count);
	for (u32 i = 0; i < image_count; i++) {
		view_create.image = p_swapchain->images[i];
		if (vkCreateImageView(m_device, &view_create, get_allocator(VK_OBJECT_TYPE_IMAGE_VIEW), &p_swapchain->image_views[i])
			!= VK_SUCCESS) {
			throw std::runtime_error("Failed to create image view");
		}
	}

	// TODO: Create framebuffers
}

void VulkanRenderDriver::destroy_swapchain(SwapchainID p_swapchain) {
	NOVA_AUTO_TRACE();
	NOVA_ASSERT(p_swapchain);

	for (const auto& framebuffer : p_swapchain->framebuffers) {
		vkDestroyFramebuffer(m_device, framebuffer, get_allocator(VK_OBJECT_TYPE_FRAMEBUFFER));
	}
	for (const auto& image_view : p_swapchain->image_views) {
		vkDestroyImageView(m_device, image_view, get_allocator(VK_OBJECT_TYPE_IMAGE_VIEW));
	}
	if (p_swapchain->handle) {
		vkDestroySwapchainKHR(m_device, p_swapchain->handle, get_allocator(VK_OBJECT_TYPE_SWAPCHAIN_KHR));
	}
	// TODO: Destroy render pass

	delete p_swapchain;
}

ShaderID VulkanRenderDriver::create_shader(const std::span<u8> p_bytes) {
	NOVA_AUTO_TRACE();
	NOVA_ASSERT(!p_bytes.empty());

	Shader* shader = new Shader();

	VkShaderModuleCreateInfo create {};
	create.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	create.codeSize = p_bytes.size();
	create.pCode = reinterpret_cast<const u32*>(p_bytes.data());

	if (vkCreateShaderModule(m_device, &create, get_allocator(VK_OBJECT_TYPE_SHADER_MODULE), &shader->handle)
		!= VK_SUCCESS) {
		throw std::runtime_error("Failed to create shader module");
	}

	NOVA_LOG("VkShaderModule created");
	return shader;
}

void VulkanRenderDriver::destroy_shader(ShaderID p_shader) {
	NOVA_AUTO_TRACE();
	NOVA_ASSERT(p_shader);
	if (p_shader->handle) {
		vkDestroyShaderModule(m_device, p_shader->handle, get_allocator(VK_OBJECT_TYPE_SHADER_MODULE));
	}
	delete p_shader;
}

PipelineID VulkanRenderDriver::create_pipeline(GraphicsPipelineParams& p_params) {
	NOVA_AUTO_TRACE();
	NOVA_ASSERT(p_params.render_pass);

	std::vector<VkPipelineShaderStageCreateInfo> shader_stages;
	for (const auto& [stage, shader] : p_params.shaders) {
		VkPipelineShaderStageCreateInfo stage_create {};
		stage_create.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		stage_create.stage = VK_SHADER_STAGE_MAP[static_cast<int>(stage)];
		stage_create.module = shader->handle;
		stage_create.pName = "main"; // TODO: Get from shader
		// TODO: Get specialization info from shader
		shader_stages.push_back(stage_create);
	}

	// TODO: Properly set up vertex input state
	VkPipelineVertexInputStateCreateInfo vertex_input {};
	vertex_input.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertex_input.vertexBindingDescriptionCount = 0;
	vertex_input.vertexAttributeDescriptionCount = 0;

	VkPipelineInputAssemblyStateCreateInfo input_assembly {};
	input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_MAP[static_cast<int>(p_params.topology)];
	input_assembly.primitiveRestartEnable = VK_FALSE;

	// TODO: Tessellation state

	VkPipelineViewportStateCreateInfo viewport {};
	viewport.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewport.viewportCount = 1; // TODO: Support VR
	viewport.scissorCount = 1;

	VkPipelineRasterizationStateCreateInfo rasterization {};
	rasterization.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterization.depthClampEnable = p_params.enable_depth_clamp;
	rasterization.rasterizerDiscardEnable = p_params.discard_primitives;
	rasterization.polygonMode = p_params.wireframe ? VK_POLYGON_MODE_LINE : VK_POLYGON_MODE_FILL;
	rasterization.cullMode = VK_CULL_MODE_MAP[static_cast<int>(p_params.cull_mode)];
	rasterization.frontFace = VK_FRONT_FACE_MAP[static_cast<int>(p_params.front_face)];
	rasterization.depthBiasEnable = p_params.enable_depth_bias;
	rasterization.depthBiasConstantFactor = p_params.depth_bias_constant;
	rasterization.depthBiasClamp = p_params.depth_bias_clamp;
	rasterization.depthBiasSlopeFactor = p_params.depth_bias_slope;
	rasterization.lineWidth = p_params.line_width;

	VkPipelineMultisampleStateCreateInfo multisample {};
	multisample.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisample.sampleShadingEnable = VK_FALSE; // TODO: Support MSAA
	multisample.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT; // TODO: Support MSAA

	// TODO: Depth stencil state

	// TODO: Properly set up color blend state
	std::vector<VkPipelineColorBlendAttachmentState> attachments;
	VkPipelineColorBlendStateCreateInfo color_blend {};
	color_blend.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	color_blend.logicOpEnable = VK_FALSE;
	color_blend.logicOp = VK_LOGIC_OP_COPY;
	color_blend.attachmentCount = static_cast<u32>(attachments.size());
	color_blend.pAttachments = attachments.data();
	color_blend.blendConstants[0] = 0.0f;
	color_blend.blendConstants[1] = 0.0f;
	color_blend.blendConstants[2] = 0.0f;
	color_blend.blendConstants[3] = 0.0f;

	std::vector<VkDynamicState> dynamic_states;
	dynamic_states.push_back(VK_DYNAMIC_STATE_VIEWPORT);
	dynamic_states.push_back(VK_DYNAMIC_STATE_SCISSOR);
	// TODO: Add more dynamic states
	VkPipelineDynamicStateCreateInfo dynamic_state {};
	dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamic_state.dynamicStateCount = static_cast<u32>(dynamic_states.size());
	dynamic_state.pDynamicStates = dynamic_states.data();

	Pipeline* pipeline = new Pipeline();

	// TODO: Move this to the shader
	VkPipelineLayoutCreateInfo layout_create {};
	layout_create.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	layout_create.setLayoutCount = 0; // TODO: Add descriptor sets
	layout_create.pushConstantRangeCount = 0; // TODO: Add push constants
	if (vkCreatePipelineLayout(m_device, &layout_create, get_allocator(VK_OBJECT_TYPE_PIPELINE_LAYOUT), &pipeline->layout)
		!= VK_SUCCESS) {
		throw std::runtime_error("Failed to create pipeline layout");
	}

	VkGraphicsPipelineCreateInfo pipeline_create {};
	pipeline_create.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipeline_create.stageCount = static_cast<u32>(shader_stages.size());
	pipeline_create.pStages = shader_stages.data();
	pipeline_create.pVertexInputState = &vertex_input;
	pipeline_create.pInputAssemblyState = &input_assembly;
	pipeline_create.pTessellationState = nullptr; // TODO: Add tessellation state
	pipeline_create.pViewportState = &viewport;
	pipeline_create.pRasterizationState = &rasterization;
	pipeline_create.pMultisampleState = &multisample;
	pipeline_create.pDepthStencilState = nullptr; // TODO: Add depth stencil state
	pipeline_create.pColorBlendState = &color_blend;
	pipeline_create.pDynamicState = &dynamic_state;
	pipeline_create.layout = pipeline->layout;
	pipeline_create.renderPass = p_params.render_pass->handle;
	pipeline_create.subpass = p_params.subpass;

	if (vkCreateGraphicsPipelines(
			m_device,
			VK_NULL_HANDLE,
			1,
			&pipeline_create,
			get_allocator(VK_OBJECT_TYPE_PIPELINE),
			&pipeline->handle
		)
		!= VK_SUCCESS) {
		throw std::runtime_error("Failed to create graphics pipeline");
	}
	return pipeline;
}

PipelineID VulkanRenderDriver::create_pipeline(ComputePipelineParams& p_params) {
	NOVA_AUTO_TRACE();
	Pipeline* pipeline = new Pipeline();
	(void)p_params;

	VkComputePipelineCreateInfo create {};
	create.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;

	if (vkCreateComputePipelines(m_device, VK_NULL_HANDLE, 1, &create, get_allocator(VK_OBJECT_TYPE_PIPELINE), &pipeline->handle)
		!= VK_SUCCESS) {
		throw std::runtime_error("Failed to create compute pipeline");
	}
	return pipeline;
}

void VulkanRenderDriver::destroy_pipeline(PipelineID p_pipeline) {
	NOVA_AUTO_TRACE();
	NOVA_ASSERT(p_pipeline);
	if (p_pipeline->layout) {
		vkDestroyPipelineLayout(m_device, p_pipeline->layout, get_allocator(VK_OBJECT_TYPE_PIPELINE_LAYOUT));
	}
	if (p_pipeline->handle) {
		vkDestroyPipeline(m_device, p_pipeline->handle, get_allocator(VK_OBJECT_TYPE_PIPELINE));
	}
	delete p_pipeline;
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
