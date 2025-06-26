/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

/// NOTE: This header should only be included in implementation files

#include <nova/render/render_driver.h>
#include <nova/render/render_structs.h>
#include <nova/types.h>

#include <vulkan/vulkan.h>

#include <string>
#include <vector>

namespace Nova {
	struct CommandBuffer {
		VkCommandBuffer handle = VK_NULL_HANDLE;
	};

	struct CommandPool {
		VkCommandPool handle = VK_NULL_HANDLE;
		std::vector<CommandBufferID> allocated_buffers;
	};

	struct Pipeline {
		PipelineType type;
		VkPipeline handle = VK_NULL_HANDLE;
		VkPipelineLayout layout = VK_NULL_HANDLE;
	};

	struct Queue {
		VkQueue handle = VK_NULL_HANDLE;
		u32 family_index;
		u32 queue_index;
		u32 usage_count = 0;
	};

	struct RenderPass {
		VkRenderPass handle = VK_NULL_HANDLE;
	};

	struct Shader {
		VkShaderModule handle = VK_NULL_HANDLE;
		ShaderStage stage = ShaderStage::VERTEX;
		std::string name;
	};

	struct Surface {
		VkSurfaceKHR handle = VK_NULL_HANDLE;
		u32 width = 0;
		u32 height = 0;
		bool dirty = false; // TODO: Use state enum
	};

	struct Swapchain {
		VkSwapchainKHR handle = VK_NULL_HANDLE;
		VkFormat format = VK_FORMAT_UNDEFINED;
		VkColorSpaceKHR color_space = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
		std::vector<VkImage> images;
		std::vector<VkImageView> image_views;
		std::vector<VkFramebuffer> framebuffers;
		SurfaceID surface = nullptr;
		RenderPassID render_pass = nullptr;
	};
} // namespace Nova
