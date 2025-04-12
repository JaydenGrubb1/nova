/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#ifdef NOVA_X11

#include <X11/Xlib.h>
#include <nova/platform/window_driver.h>

#include <unordered_map>

namespace Nova {
	struct WindowData {
		int width = 0;
		int height = 0;
	};

	class X11WindowDriver final : public WindowDriver {
	  public:
		X11WindowDriver();
		~X11WindowDriver() override;

		[[nodiscard]] WindowAPI get_api() const override;
		[[nodiscard]] std::string get_api_name() const override;

		void poll_events() override;
		void beep() override;

		[[nodiscard]] u32 get_window_count() const override;
		[[nodiscard]] WindowID create_window(std::string_view title, u32 width, u32 height) override;
		void destroy_window(WindowID window) override;

		void set_window_title(WindowID window, std::string_view title) override;
		void set_window_size(WindowID window, u32 width, u32 height) override;
		void set_window_position(WindowID window, i32 x, i32 y) override;

		[[nodiscard]] const char* get_surface_extension() const override;
		[[nodiscard]] SurfaceID create_surface(WindowID window, RenderDriver* render_driver) override;

	  private:
		Display* m_display = nullptr;
		std::unordered_map<WindowID, WindowData> m_windows;

		Atom m_window_close_atom = 0;
	};
} // namespace Nova

#endif // NOVA_X11
