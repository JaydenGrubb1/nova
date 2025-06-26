/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

namespace Nova {
	struct CommandBuffer;
	struct CommandPool;
	struct Pipeline;
	struct Queue;
	struct RenderPass;
	struct Shader;
	struct Surface;
	struct Swapchain;

	using CommandBufferID = CommandBuffer*;
	using CommandPoolID = CommandPool*;
	using PipelineID = Pipeline*;
	using QueueID = Queue*;
	using RenderPassID = RenderPass*;
	using ShaderID = Shader*;
	using SurfaceID = Surface*;
	using SwapchainID = Swapchain*;
} // namespace Nova
