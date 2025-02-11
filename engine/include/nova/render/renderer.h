/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#include <nova/api.h>
#include <nova/render/render_api.h>
#include <nova/render/render_driver.h>

#include <memory>

namespace Nova {
	class NOVA_API Renderer {
	  public:
		static void create(RenderAPI api);
		static void shutdown();
		static Renderer* get();
		static RenderDriver* get_driver();

	  private:
		std::unique_ptr<RenderDriver> m_driver;

		Renderer() = default;
		~Renderer() = default;
	};
} // namespace Nova
