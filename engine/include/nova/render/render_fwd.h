/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

namespace Nova {
	enum class CullMode { NONE, FRONT, BACK };
	enum class FrontFace { CLOCKWISE, COUNTER_CLOCKWISE };
	enum class PipelineType { GRAPHICS, COMPUTE };
	enum class PrimitiveTopology { POINT_LIST, LINE_LIST, LINE_STRIP, TRIANGLE_LIST, TRIANGLE_STRIP };
	enum class RenderAPI { DX12, VULKAN };
	enum class ShaderStage { VERTEX, FRAGMENT, GEOMETRY, TESS_CONTROL, TESS_EVAL, COMPUTE, MESH, TASK };

	class RenderDriver;
	struct RenderDevice;

	struct GraphicsPipelineParams;
	struct ComputePipelineParams;

	struct Pipeline;
	struct RenderPass;
	struct Shader;
	struct Surface;
	struct Swapchain;

	using PipelineID = Pipeline*;
	using RenderPassID = RenderPass*;
	using ShaderID = Shader*;
	using SurfaceID = Surface*;
	using SwapchainID = Swapchain*;
} // namespace Nova
