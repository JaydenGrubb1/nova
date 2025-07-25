/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#include <nova/render/data_format.h>
#include <nova/render/render_structs.h>
#include <nova/types.h>

#include <vector>

namespace Nova {
	enum class CullMode { NONE, FRONT, BACK };
	enum class FrontFace { CLOCKWISE, COUNTER_CLOCKWISE };
	enum class InputRate { VERTEX, INSTANCE };
	enum class PrimitiveTopology { POINT_LIST, LINE_LIST, LINE_STRIP, TRIANGLE_LIST, TRIANGLE_STRIP };

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
		std::vector<ShaderID> shaders;
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
} // namespace Nova
