/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#include <nova/render/render_fwd.h>
#include <nova/types.h>

#include <unordered_map>
#include <vector>

namespace Nova {
	struct VertexAttribute {
		u32 binding = 0;
		u32 location = 0;
		u32 offset = 0;
		DataFormat format = DataFormat::R32G32B32_SFLOAT;
	};

	struct VertexBinding {
		u32 binding = 0;
		u32 stride = 0;
		InputRate rate = InputRate::VERTEX;
	};

	struct GraphicsPipelineParams {
		std::unordered_map<ShaderStage, ShaderID> shaders;
		std::vector<VertexBinding> bindings;
		std::vector<VertexAttribute> attributes;
		PrimitiveTopology topology = PrimitiveTopology::TRIANGLE_LIST;

		// TODO: Tessellation state

		bool enable_depth_clamp = false;
		bool discard_primitives = false;
		bool wireframe = false;
		CullMode cull_mode = CullMode::NONE;
		FrontFace front_face = FrontFace::COUNTER_CLOCKWISE;
		bool enable_depth_bias = false;
		float depth_bias_constant = 0.0f;
		float depth_bias_clamp = 0.0f;
		float depth_bias_slope = 0.0f;
		float line_width = 1.0f;

		// TODO: Multisample state
		// TODO: Depth stencil state
		// TODO: Color blend state
		// TODO: Dynamic state

		RenderPassID render_pass = nullptr;
		u32 subpass = 0;
	};
	struct ComputePipelineParams {};
} // namespace Nova
