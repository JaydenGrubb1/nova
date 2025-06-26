/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#include <nova/api.h>
#include <nova/platform/platform_structs.h>
#include <nova/render/params/compute_pipeline.h>
#include <nova/render/params/graphics_pipeline.h>
#include <nova/render/params/render_pass.h>
#include <nova/render/render_device.h>
#include <nova/render/render_structs.h>
#include <nova/types.h>

#include <span>
#include <string>

namespace Nova {
	class WindowDriver;

	enum class PipelineType { GRAPHICS, COMPUTE };
	enum class QueueType { UNDEFINED, GRAPHICS, COMPUTE, TRANSFER };
	enum class RenderAPI { DX12, VULKAN };
	enum class ShaderStage { VERTEX, FRAGMENT, GEOMETRY, TESS_CONTROL, TESS_EVAL, COMPUTE, MESH, TASK };

	class NOVA_API RenderDriver {
	  public:
		static RenderDriver* create(RenderAPI api, WindowDriver* window_driver = nullptr);
		virtual ~RenderDriver() = default;

		virtual RenderAPI get_api() const = 0;
		virtual u32 get_api_version() const = 0;
		virtual std::string get_api_name() const = 0;
		virtual std::string get_api_version_string() const = 0;

		virtual u32 get_device_count() const = 0;
		virtual const RenderDevice& get_device(u32 index) const = 0;
		virtual bool get_device_supports_surface(u32 index, SurfaceID surface) const = 0;
		virtual void select_device(u32 index) = 0;

		virtual u32 choose_queue_family(QueueType type, SurfaceID surface) = 0;

		[[nodiscard]] virtual QueueID get_queue(u32 queue_family) = 0;
		virtual void free_queue(QueueID queue) = 0;

		[[nodiscard]] virtual SurfaceID create_surface(WindowID window) = 0;
		virtual void destroy_surface(SurfaceID surface) = 0;

		[[nodiscard]] virtual SwapchainID create_swapchain(SurfaceID surface) = 0;
		virtual void resize_swapchain(SwapchainID swapchain) = 0;
		virtual RenderPassID get_swapchain_render_pass(SwapchainID swapchain) const = 0;
		virtual void destroy_swapchain(SwapchainID swapchain) = 0;

		[[nodiscard]] virtual ShaderID create_shader(const std::span<u8> bytes, ShaderStage stage) = 0;
		virtual void destroy_shader(ShaderID shader) = 0;

		[[nodiscard]] virtual RenderPassID create_render_pass(RenderPassParams& params) = 0;
		virtual void destroy_render_pass(RenderPassID render_pass) = 0;

		[[nodiscard]] virtual PipelineID create_pipeline(GraphicsPipelineParams& params) = 0;
		[[nodiscard]] virtual PipelineID create_pipeline(ComputePipelineParams& params) = 0;
		virtual void destroy_pipeline(PipelineID pipeline) = 0;

		[[nodiscard]] virtual CommandPoolID create_command_pool(QueueID queue) = 0;
		virtual void destroy_command_pool(CommandPoolID command_pool) = 0;

		[[nodiscard]] virtual CommandBufferID create_command_buffer(CommandPoolID pool) = 0;
		virtual void begin_command_buffer(CommandBufferID command_buffer) = 0;
		virtual void end_command_buffer(CommandBufferID command_buffer) = 0;
	};
} // namespace Nova
