/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#include <nova/api.h>
#include <nova/render/render_api.h>
#include <nova/render/render_driver.h>

namespace Nova {
	class NOVA_API Renderer {
	  public:
		static void init(RenderAPI api);
		static void shutdown();
		static RenderDriver* get_driver();
	};
} // namespace Nova
