/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#include <nova/api.h>
#include <nova/platform/window_fwd.h>
#include <nova/render/render_fwd.h>
#include <nova/types.h>

#include <string_view>

namespace Nova {
	class NOVA_API WindowDriver {
	  public:
		static WindowDriver* create();
		virtual ~WindowDriver() = default;

		[[nodiscard]] virtual WindowAPI get_api() const = 0;
		[[nodiscard]] virtual std::string get_api_name() const = 0;

		virtual void poll_events() = 0;
		virtual void beep() = 0;

		[[nodiscard]] virtual u32 get_window_count() const = 0;
		[[nodiscard]] virtual WindowID create_window(std::string_view title, u32 width, u32 height) = 0;
		virtual void destroy_window(WindowID window) = 0;

		virtual void set_window_title(WindowID window, std::string_view title) = 0;
		virtual void set_window_size(WindowID window, u32 width, u32 height) = 0;
		virtual void set_window_position(WindowID window, i32 x, i32 y) = 0;

		[[nodiscard]] virtual const char* get_surface_extension() const = 0;
		[[nodiscard]] virtual SurfaceID create_surface(WindowID window, RenderDriver* driver) = 0;
	};
} // namespace Nova
