/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#include <nova/api.h>
#include <nova/render/render_api.h>
#include <nova/types.h>

namespace Nova {
	class NOVA_API RenderDriver {
	  public:
		virtual ~RenderDriver() = default;

		[[nodiscard]] virtual RenderAPI get_api() const = 0;
		[[nodiscard]] virtual u32 get_api_version() const = 0;
	};
} // namespace Nova
