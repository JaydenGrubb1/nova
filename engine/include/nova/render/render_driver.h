/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#include <nova/api.h>
#include <nova/render/render_api.h>
#include <nova/render/render_device.h>
#include <nova/types.h>

#include <string>

namespace Nova {
	using WindowID = uptr;
	using SurfaceID = uptr;

	class WindowDriver;

	class NOVA_API RenderDriver {
	  public:
		static RenderDriver* create(RenderAPI api, WindowDriver* window_driver = nullptr);
		virtual ~RenderDriver() = default;

		[[nodiscard]] virtual RenderAPI get_api() const = 0;
		[[nodiscard]] virtual u32 get_api_version() const = 0;
		[[nodiscard]] virtual std::string get_api_name() const = 0;
		[[nodiscard]] virtual std::string get_api_version_string() const = 0;

		[[nodiscard]] virtual u32 get_device_count() const = 0;
		[[nodiscard]] virtual const RenderDevice& get_device(u32 index) const = 0;
		virtual void select_device(u32 index) = 0;

		[[nodiscard]] virtual SurfaceID create_surface(WindowID window) = 0;
		virtual void destroy_surface(SurfaceID surface) = 0;
		// TODO: get_surface_size
		// TODO: get_surface_mode
		// TODO: get_surface_state
		// TODO: set_surface_size
		// TODO: set_surface_mode
		// TODO: set_surface_state
	};
} // namespace Nova
