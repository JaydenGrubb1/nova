/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

namespace Nova {
	enum class RenderAPI { DX12, VULKAN };

	class RenderDriver;
	struct RenderDevice;

	struct Surface;
	using SurfaceID = Surface*;
} // namespace Nova
