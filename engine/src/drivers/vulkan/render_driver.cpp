/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifdef NOVA_VULKAN

#include "drivers/vulkan/render_driver.h"

#include "drivers/vulkan/render_structs.h"

#include <nova/core/debug.h>
#include <nova/platform/window_driver.h>
#include <nova/render/render_device.h>
#include <nova/version.h>

#include <vulkan/vulkan.h>

#include <algorithm>
#include <bit>
#include <format>
#include <limits>
#include <string_view>

namespace {
	static constexpr u32 MAX_QUEUES_PER_FAMILY = 2;
	static constexpr std::string_view VALIDATION_LAYER = "VK_LAYER_KHRONOS_validation";

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
		VK_CULL_MODE_BACK_BIT,
	};

	static constexpr VkFrontFace VK_FRONT_FACE_MAP[] = {
		VK_FRONT_FACE_COUNTER_CLOCKWISE,
		VK_FRONT_FACE_CLOCKWISE,
	};

	static constexpr VkVertexInputRate VK_VERTEX_INPUT_RATE_MAP[] = {
		VK_VERTEX_INPUT_RATE_VERTEX,
		VK_VERTEX_INPUT_RATE_INSTANCE,
	};

	static constexpr VkQueueFlagBits VK_QUEUE_FLAGS_MAP[] = {
		static_cast<VkQueueFlagBits>(0),
		VK_QUEUE_GRAPHICS_BIT,
		VK_QUEUE_COMPUTE_BIT,
		VK_QUEUE_TRANSFER_BIT,
	};

	static constexpr VkFormat VK_FORMAT_MAP[] = {
		VK_FORMAT_UNDEFINED,
		VK_FORMAT_R4G4_UNORM_PACK8,
		VK_FORMAT_R4G4B4A4_UNORM_PACK16,
		VK_FORMAT_B4G4R4A4_UNORM_PACK16,
		VK_FORMAT_R5G6B5_UNORM_PACK16,
		VK_FORMAT_B5G6R5_UNORM_PACK16,
		VK_FORMAT_R5G5B5A1_UNORM_PACK16,
		VK_FORMAT_B5G5R5A1_UNORM_PACK16,
		VK_FORMAT_A1R5G5B5_UNORM_PACK16,
		VK_FORMAT_R8_UNORM,
		VK_FORMAT_R8_SNORM,
		VK_FORMAT_R8_USCALED,
		VK_FORMAT_R8_SSCALED,
		VK_FORMAT_R8_UINT,
		VK_FORMAT_R8_SINT,
		VK_FORMAT_R8_SRGB,
		VK_FORMAT_R8G8_UNORM,
		VK_FORMAT_R8G8_SNORM,
		VK_FORMAT_R8G8_USCALED,
		VK_FORMAT_R8G8_SSCALED,
		VK_FORMAT_R8G8_UINT,
		VK_FORMAT_R8G8_SINT,
		VK_FORMAT_R8G8_SRGB,
		VK_FORMAT_R8G8B8_UNORM,
		VK_FORMAT_R8G8B8_SNORM,
		VK_FORMAT_R8G8B8_USCALED,
		VK_FORMAT_R8G8B8_SSCALED,
		VK_FORMAT_R8G8B8_UINT,
		VK_FORMAT_R8G8B8_SINT,
		VK_FORMAT_R8G8B8_SRGB,
		VK_FORMAT_B8G8R8_UNORM,
		VK_FORMAT_B8G8R8_SNORM,
		VK_FORMAT_B8G8R8_USCALED,
		VK_FORMAT_B8G8R8_SSCALED,
		VK_FORMAT_B8G8R8_UINT,
		VK_FORMAT_B8G8R8_SINT,
		VK_FORMAT_B8G8R8_SRGB,
		VK_FORMAT_R8G8B8A8_UNORM,
		VK_FORMAT_R8G8B8A8_SNORM,
		VK_FORMAT_R8G8B8A8_USCALED,
		VK_FORMAT_R8G8B8A8_SSCALED,
		VK_FORMAT_R8G8B8A8_UINT,
		VK_FORMAT_R8G8B8A8_SINT,
		VK_FORMAT_R8G8B8A8_SRGB,
		VK_FORMAT_B8G8R8A8_UNORM,
		VK_FORMAT_B8G8R8A8_SNORM,
		VK_FORMAT_B8G8R8A8_USCALED,
		VK_FORMAT_B8G8R8A8_SSCALED,
		VK_FORMAT_B8G8R8A8_UINT,
		VK_FORMAT_B8G8R8A8_SINT,
		VK_FORMAT_B8G8R8A8_SRGB,
		VK_FORMAT_A8B8G8R8_UNORM_PACK32,
		VK_FORMAT_A8B8G8R8_SNORM_PACK32,
		VK_FORMAT_A8B8G8R8_USCALED_PACK32,
		VK_FORMAT_A8B8G8R8_SSCALED_PACK32,
		VK_FORMAT_A8B8G8R8_UINT_PACK32,
		VK_FORMAT_A8B8G8R8_SINT_PACK32,
		VK_FORMAT_A8B8G8R8_SRGB_PACK32,
		VK_FORMAT_A2R10G10B10_UNORM_PACK32,
		VK_FORMAT_A2R10G10B10_SNORM_PACK32,
		VK_FORMAT_A2R10G10B10_USCALED_PACK32,
		VK_FORMAT_A2R10G10B10_SSCALED_PACK32,
		VK_FORMAT_A2R10G10B10_UINT_PACK32,
		VK_FORMAT_A2R10G10B10_SINT_PACK32,
		VK_FORMAT_A2B10G10R10_UNORM_PACK32,
		VK_FORMAT_A2B10G10R10_SNORM_PACK32,
		VK_FORMAT_A2B10G10R10_USCALED_PACK32,
		VK_FORMAT_A2B10G10R10_SSCALED_PACK32,
		VK_FORMAT_A2B10G10R10_UINT_PACK32,
		VK_FORMAT_A2B10G10R10_SINT_PACK32,
		VK_FORMAT_R16_UNORM,
		VK_FORMAT_R16_SNORM,
		VK_FORMAT_R16_USCALED,
		VK_FORMAT_R16_SSCALED,
		VK_FORMAT_R16_UINT,
		VK_FORMAT_R16_SINT,
		VK_FORMAT_R16_SFLOAT,
		VK_FORMAT_R16G16_UNORM,
		VK_FORMAT_R16G16_SNORM,
		VK_FORMAT_R16G16_USCALED,
		VK_FORMAT_R16G16_SSCALED,
		VK_FORMAT_R16G16_UINT,
		VK_FORMAT_R16G16_SINT,
		VK_FORMAT_R16G16_SFLOAT,
		VK_FORMAT_R16G16B16_UNORM,
		VK_FORMAT_R16G16B16_SNORM,
		VK_FORMAT_R16G16B16_USCALED,
		VK_FORMAT_R16G16B16_SSCALED,
		VK_FORMAT_R16G16B16_UINT,
		VK_FORMAT_R16G16B16_SINT,
		VK_FORMAT_R16G16B16_SFLOAT,
		VK_FORMAT_R16G16B16A16_UNORM,
		VK_FORMAT_R16G16B16A16_SNORM,
		VK_FORMAT_R16G16B16A16_USCALED,
		VK_FORMAT_R16G16B16A16_SSCALED,
		VK_FORMAT_R16G16B16A16_UINT,
		VK_FORMAT_R16G16B16A16_SINT,
		VK_FORMAT_R16G16B16A16_SFLOAT,
		VK_FORMAT_R32_UINT,
		VK_FORMAT_R32_SINT,
		VK_FORMAT_R32_SFLOAT,
		VK_FORMAT_R32G32_UINT,
		VK_FORMAT_R32G32_SINT,
		VK_FORMAT_R32G32_SFLOAT,
		VK_FORMAT_R32G32B32_UINT,
		VK_FORMAT_R32G32B32_SINT,
		VK_FORMAT_R32G32B32_SFLOAT,
		VK_FORMAT_R32G32B32A32_UINT,
		VK_FORMAT_R32G32B32A32_SINT,
		VK_FORMAT_R32G32B32A32_SFLOAT,
		VK_FORMAT_R64_UINT,
		VK_FORMAT_R64_SINT,
		VK_FORMAT_R64_SFLOAT,
		VK_FORMAT_R64G64_UINT,
		VK_FORMAT_R64G64_SINT,
		VK_FORMAT_R64G64_SFLOAT,
		VK_FORMAT_R64G64B64_UINT,
		VK_FORMAT_R64G64B64_SINT,
		VK_FORMAT_R64G64B64_SFLOAT,
		VK_FORMAT_R64G64B64A64_UINT,
		VK_FORMAT_R64G64B64A64_SINT,
		VK_FORMAT_R64G64B64A64_SFLOAT,
		VK_FORMAT_B10G11R11_UFLOAT_PACK32,
		VK_FORMAT_E5B9G9R9_UFLOAT_PACK32,
		VK_FORMAT_D16_UNORM,
		VK_FORMAT_X8_D24_UNORM_PACK32,
		VK_FORMAT_D32_SFLOAT,
		VK_FORMAT_S8_UINT,
		VK_FORMAT_D16_UNORM_S8_UINT,
		VK_FORMAT_D24_UNORM_S8_UINT,
		VK_FORMAT_D32_SFLOAT_S8_UINT,
		VK_FORMAT_BC1_RGB_UNORM_BLOCK,
		VK_FORMAT_BC1_RGB_SRGB_BLOCK,
		VK_FORMAT_BC1_RGBA_UNORM_BLOCK,
		VK_FORMAT_BC1_RGBA_SRGB_BLOCK,
		VK_FORMAT_BC2_UNORM_BLOCK,
		VK_FORMAT_BC2_SRGB_BLOCK,
		VK_FORMAT_BC3_UNORM_BLOCK,
		VK_FORMAT_BC3_SRGB_BLOCK,
		VK_FORMAT_BC4_UNORM_BLOCK,
		VK_FORMAT_BC4_SNORM_BLOCK,
		VK_FORMAT_BC5_UNORM_BLOCK,
		VK_FORMAT_BC5_SNORM_BLOCK,
		VK_FORMAT_BC6H_UFLOAT_BLOCK,
		VK_FORMAT_BC6H_SFLOAT_BLOCK,
		VK_FORMAT_BC7_UNORM_BLOCK,
		VK_FORMAT_BC7_SRGB_BLOCK,
		VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK,
		VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK,
		VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK,
		VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK,
		VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK,
		VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK,
		VK_FORMAT_EAC_R11_UNORM_BLOCK,
		VK_FORMAT_EAC_R11_SNORM_BLOCK,
		VK_FORMAT_EAC_R11G11_UNORM_BLOCK,
		VK_FORMAT_EAC_R11G11_SNORM_BLOCK,
		VK_FORMAT_ASTC_4x4_UNORM_BLOCK,
		VK_FORMAT_ASTC_4x4_SRGB_BLOCK,
		VK_FORMAT_ASTC_5x4_UNORM_BLOCK,
		VK_FORMAT_ASTC_5x4_SRGB_BLOCK,
		VK_FORMAT_ASTC_5x5_UNORM_BLOCK,
		VK_FORMAT_ASTC_5x5_SRGB_BLOCK,
		VK_FORMAT_ASTC_6x5_UNORM_BLOCK,
		VK_FORMAT_ASTC_6x5_SRGB_BLOCK,
		VK_FORMAT_ASTC_6x6_UNORM_BLOCK,
		VK_FORMAT_ASTC_6x6_SRGB_BLOCK,
		VK_FORMAT_ASTC_8x5_UNORM_BLOCK,
		VK_FORMAT_ASTC_8x5_SRGB_BLOCK,
		VK_FORMAT_ASTC_8x6_UNORM_BLOCK,
		VK_FORMAT_ASTC_8x6_SRGB_BLOCK,
		VK_FORMAT_ASTC_8x8_UNORM_BLOCK,
		VK_FORMAT_ASTC_8x8_SRGB_BLOCK,
		VK_FORMAT_ASTC_10x5_UNORM_BLOCK,
		VK_FORMAT_ASTC_10x5_SRGB_BLOCK,
		VK_FORMAT_ASTC_10x6_UNORM_BLOCK,
		VK_FORMAT_ASTC_10x6_SRGB_BLOCK,
		VK_FORMAT_ASTC_10x8_UNORM_BLOCK,
		VK_FORMAT_ASTC_10x8_SRGB_BLOCK,
		VK_FORMAT_ASTC_10x10_UNORM_BLOCK,
		VK_FORMAT_ASTC_10x10_SRGB_BLOCK,
		VK_FORMAT_ASTC_12x10_UNORM_BLOCK,
		VK_FORMAT_ASTC_12x10_SRGB_BLOCK,
		VK_FORMAT_ASTC_12x12_UNORM_BLOCK,
		VK_FORMAT_ASTC_12x12_SRGB_BLOCK,
		VK_FORMAT_G8B8G8R8_422_UNORM,
		VK_FORMAT_B8G8R8G8_422_UNORM,
		VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM,
		VK_FORMAT_G8_B8R8_2PLANE_420_UNORM,
		VK_FORMAT_G8_B8_R8_3PLANE_422_UNORM,
		VK_FORMAT_G8_B8R8_2PLANE_422_UNORM,
		VK_FORMAT_G8_B8_R8_3PLANE_444_UNORM,
		VK_FORMAT_R10X6_UNORM_PACK16,
		VK_FORMAT_R10X6G10X6_UNORM_2PACK16,
		VK_FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16,
		VK_FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16,
		VK_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16,
		VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16,
		VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16,
		VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16,
		VK_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16,
		VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16,
		VK_FORMAT_R12X4_UNORM_PACK16,
		VK_FORMAT_R12X4G12X4_UNORM_2PACK16,
		VK_FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16,
		VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16,
		VK_FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16,
		VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16,
		VK_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16,
		VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16,
		VK_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16,
		VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16,
		VK_FORMAT_G16B16G16R16_422_UNORM,
		VK_FORMAT_B16G16R16G16_422_UNORM,
		VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM,
		VK_FORMAT_G16_B16R16_2PLANE_420_UNORM,
		VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM,
		VK_FORMAT_G16_B16R16_2PLANE_422_UNORM,
		VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM
	};
} // namespace

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

	VkPhysicalDevice physical_device = static_cast<VkPhysicalDevice>(m_devices[p_index].handle);

	u32 count;
	vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &count, nullptr);

	for (u32 i = 0; i < count; i++) {
		VkBool32 supports_present = VK_FALSE;
		if (vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, i, p_surface->handle, &supports_present)
			!= VK_SUCCESS) {
			continue;
		}
		if (supports_present) {
			return true;
		}
	}

	return false;
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

u32 VulkanRenderDriver::choose_queue_family(QueueType p_type, SurfaceID p_surface) {
	NOVA_AUTO_TRACE();
	NOVA_ASSERT(!m_queue_families.empty());

	const VkQueueFlags mask = VK_QUEUE_FLAGS_MAP[static_cast<int>(p_type)];

	u32 best_index = std::numeric_limits<u32>::max();
	u32 best_score = std::numeric_limits<u32>::max();

	for (const auto [index, flags] : m_queue_families) {
		if ((flags & mask) != mask) {
			continue;
		}
		if (p_surface) {
			VkBool32 supports_present = VK_FALSE;
			if (vkGetPhysicalDeviceSurfaceSupportKHR(m_physical_device, index, p_surface->handle, &supports_present)
				!= VK_SUCCESS) {
				continue;
			}
			if (!supports_present) {
				continue;
			}
		}

		u32 score = std::popcount(flags);
		if (score < best_score) {
			best_index = index;
			best_score = score;
		}
	}

	return best_index;
}

QueueID VulkanRenderDriver::get_queue(u32 p_queue_family) {
	NOVA_AUTO_TRACE();
	NOVA_ASSERT(!m_queues.empty());

	QueueID best_queue = nullptr;
	u32 best_usage = std::numeric_limits<u32>::max();

	for (Queue& queue : m_queues) {
		if (queue.family_index != p_queue_family) {
			continue;
		}
		if (queue.usage_count < best_usage) {
			best_queue = &queue;
			best_usage = queue.usage_count;
		}
	}

	if (!best_queue) {
		throw std::runtime_error("Failed to find a queue");
	}

	best_queue->usage_count++;
	return best_queue;
}

void VulkanRenderDriver::free_queue(QueueID p_queue) {
	NOVA_AUTO_TRACE();
	NOVA_ASSERT(p_queue);
	p_queue->usage_count--;
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

	VkAttachmentDescription attachment {};
	attachment.format = swapchain->format;
	attachment.samples = VK_SAMPLE_COUNT_1_BIT;
	attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference color_ref {};
	color_ref.attachment = 0;
	color_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &color_ref;

	VkRenderPassCreateInfo pass_create {};
	pass_create.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	pass_create.attachmentCount = 1;
	pass_create.pAttachments = &attachment;
	pass_create.subpassCount = 1;
	pass_create.pSubpasses = &subpass;

	swapchain->render_pass = new RenderPass();
	if (vkCreateRenderPass(m_device, &pass_create, get_allocator(VK_OBJECT_TYPE_RENDER_PASS), &swapchain->render_pass->handle)
		!= VK_SUCCESS) {
		throw std::runtime_error("Failed to create render pass");
	}

	// TODO: Change VkRenderPass to VkRenderPass2KHR (Vulkan 1.2+)

	resize_swapchain(swapchain);
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

	VkFramebufferCreateInfo fb_create {};
	fb_create.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	fb_create.renderPass = p_swapchain->render_pass->handle;
	fb_create.attachmentCount = 1;
	fb_create.width = extent.width;
	fb_create.height = extent.height;
	fb_create.layers = 1; // TODO: Support VR

	p_swapchain->framebuffers.resize(image_count);
	for (u32 i = 0; i < image_count; i++) {
		fb_create.pAttachments = &p_swapchain->image_views[i];
		if (vkCreateFramebuffer(
				m_device,
				&fb_create,
				get_allocator(VK_OBJECT_TYPE_FRAMEBUFFER),
				&p_swapchain->framebuffers[i]
			)
			!= VK_SUCCESS) {
			throw std::runtime_error("Failed to create framebuffer");
		}
	}
}

RenderPassID VulkanRenderDriver::get_swapchain_render_pass(SwapchainID p_swapchain) const {
	NOVA_ASSERT(p_swapchain);
	return p_swapchain->render_pass;
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
	if (p_swapchain->render_pass) {
		destroy_render_pass(p_swapchain->render_pass);
	}

	delete p_swapchain;
}

ShaderID VulkanRenderDriver::create_shader(const std::span<u8> p_bytes, ShaderStage p_stage) {
	NOVA_AUTO_TRACE();
	NOVA_ASSERT(!p_bytes.empty());

	Shader* shader = new Shader();
	shader->stage = p_stage; // TODO: Get from shader code
	shader->name = "main"; // TODO: Get from shader code

	VkShaderModuleCreateInfo create {};
	create.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	create.codeSize = p_bytes.size();
	create.pCode = reinterpret_cast<const u32*>(p_bytes.data());

	if (vkCreateShaderModule(m_device, &create, get_allocator(VK_OBJECT_TYPE_SHADER_MODULE), &shader->handle)
		!= VK_SUCCESS) {
		throw std::runtime_error("Failed to create shader module");
	}

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

RenderPassID VulkanRenderDriver::create_render_pass(RenderPassParams& p_params) {
	NOVA_AUTO_TRACE();
	NOVA_WARN("{}() not implemented", NOVA_FUNC_NAME);
	RenderPass* render_pass = new RenderPass();
	(void)p_params;
	return render_pass;
}

void VulkanRenderDriver::destroy_render_pass(RenderPassID p_render_pass) {
	NOVA_AUTO_TRACE();
	NOVA_ASSERT(p_render_pass);
	if (p_render_pass->handle) {
		vkDestroyRenderPass(m_device, p_render_pass->handle, get_allocator(VK_OBJECT_TYPE_RENDER_PASS));
	}
	delete p_render_pass;
}

PipelineID VulkanRenderDriver::create_pipeline(GraphicsPipelineParams& p_params) {
	NOVA_AUTO_TRACE();
	NOVA_ASSERT(p_params.render_pass);

	std::vector<VkPipelineShaderStageCreateInfo> shader_stages;
	for (const auto& shader : p_params.shaders) {
		VkPipelineShaderStageCreateInfo stage_create {};
		stage_create.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		stage_create.stage = VK_SHADER_STAGE_MAP[static_cast<int>(shader->stage)];
		stage_create.module = shader->handle;
		stage_create.pName = shader->name.c_str();
		// TODO: Get specialization info from shader
		shader_stages.push_back(stage_create);
	}

	std::vector<VkVertexInputBindingDescription> vertex_bindings;
	for (const auto& binding : p_params.bindings) {
		VkVertexInputBindingDescription binding_desc {};
		binding_desc.binding = binding.binding;
		binding_desc.stride = binding.stride;
		binding_desc.inputRate = VK_VERTEX_INPUT_RATE_MAP[static_cast<int>(binding.rate)];
		vertex_bindings.push_back(binding_desc);
	}
	std::vector<VkVertexInputAttributeDescription> vertex_attributes;
	for (const auto& attribute : p_params.attributes) {
		VkVertexInputAttributeDescription attribute_desc {};
		attribute_desc.binding = attribute.binding;
		attribute_desc.location = attribute.location;
		attribute_desc.format = VK_FORMAT_MAP[static_cast<int>(attribute.format)];
		attribute_desc.offset = attribute.offset;
		vertex_attributes.push_back(attribute_desc);
	}
	VkPipelineVertexInputStateCreateInfo vertex_input {};
	vertex_input.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertex_input.vertexBindingDescriptionCount = static_cast<u32>(vertex_bindings.size());
	vertex_input.pVertexBindingDescriptions = vertex_bindings.data();
	vertex_input.vertexAttributeDescriptionCount = static_cast<u32>(vertex_attributes.size());
	vertex_input.pVertexAttributeDescriptions = vertex_attributes.data();

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
	attachments.emplace_back();
	attachments.back().colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT
		| VK_COLOR_COMPONENT_A_BIT;
	attachments.back().blendEnable = VK_FALSE;
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

CommandPoolID VulkanRenderDriver::create_command_pool(QueueID p_queue) {
	NOVA_AUTO_TRACE();
	NOVA_ASSERT(p_queue);
	CommandPool* pool = new CommandPool();

	VkCommandPoolCreateInfo create {};
	create.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	create.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT; // TODO: Support other pool type
	create.queueFamilyIndex = p_queue->family_index;

	if (vkCreateCommandPool(m_device, &create, get_allocator(VK_OBJECT_TYPE_COMMAND_POOL), &pool->handle) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create command pool");
	}

	return pool;
}

void VulkanRenderDriver::destroy_command_pool(CommandPoolID p_command_pool) {
	NOVA_AUTO_TRACE();
	NOVA_ASSERT(p_command_pool);
	if (p_command_pool->handle) {
		vkDestroyCommandPool(m_device, p_command_pool->handle, get_allocator(VK_OBJECT_TYPE_COMMAND_POOL));
	}
	for (const CommandBufferID buffer : p_command_pool->allocated_buffers) {
		delete buffer;
	}
	delete p_command_pool;
}

CommandBufferID VulkanRenderDriver::create_command_buffer(CommandPoolID p_pool) {
	NOVA_AUTO_TRACE();
	NOVA_ASSERT(p_pool);
	CommandBuffer* buffer = new CommandBuffer();

	VkCommandBufferAllocateInfo alloc {};
	alloc.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	alloc.commandPool = p_pool->handle;
	alloc.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY; // TODO: Support other buffer levels
	alloc.commandBufferCount = 1;

	if (vkAllocateCommandBuffers(m_device, &alloc, &buffer->handle) != VK_SUCCESS) {
		throw std::runtime_error("Failed to allocate command buffer");
	}

	p_pool->allocated_buffers.push_back(buffer);
	return buffer;
}

void VulkanRenderDriver::begin_command_buffer(CommandBufferID p_command_buffer) {
	NOVA_ASSERT(p_command_buffer);
	VkCommandBufferBeginInfo info {};
	info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	// TODO: Support flag options
	vkBeginCommandBuffer(p_command_buffer->handle, &info);
	// TODO: Check result
};

void VulkanRenderDriver::end_command_buffer(CommandBufferID p_command_buffer) {
	vkEndCommandBuffer(p_command_buffer->handle);
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
			m_layers.push_back(VALIDATION_LAYER.data());
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

		m_devices.emplace_back<RenderDevice>({
			.name = properties.deviceName,
			.vendor = static_cast<DeviceVendor>(properties.vendorID),
			.type = static_cast<DeviceType>(properties.deviceType),
			.deviceID = properties.deviceID,
			.handle = device,
		});

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

void VulkanRenderDriver::_init_queues(std::vector<VkDeviceQueueCreateInfo>& p_queues) {
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

		VkDeviceQueueCreateInfo create {};
		create.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		create.queueFamilyIndex = i;
		create.queueCount = std::min(MAX_QUEUES_PER_FAMILY, available[i].queueCount);
		create.pQueuePriorities = &s_priority;

		p_queues.push_back(create);
		m_queue_families[i] = available[i].queueFlags;

		for (u32 j = 0; j < create.queueCount; j++) {
			Queue queue;
			queue.family_index = i;
			queue.queue_index = j;
			m_queues.push_back(queue);
		}
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

	if (vkCreateDevice(m_physical_device, &create, get_allocator(VK_OBJECT_TYPE_DEVICE), &m_device) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create VkDevice");
	}

	for (Queue& queue : m_queues) {
		vkGetDeviceQueue(m_device, queue.family_index, queue.queue_index, &queue.handle);
	}
}

#endif // NOVA_VULKAN
