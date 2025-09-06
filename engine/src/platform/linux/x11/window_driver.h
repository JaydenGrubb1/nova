/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#ifdef NOVA_X11

#include "platform/linux/x11/wrapper.h"

#include <nova/platform/window_driver.h>

#include <unordered_map>

namespace Nova {
	class X11WindowDriver final : public WindowDriver {
	  public:
		X11WindowDriver();
		~X11WindowDriver() override;

		WindowAPI get_api() const override;
		std::string get_api_name() const override;

		void poll_events() override;
		void beep() override;

		u32 get_window_count() const override;
		[[nodiscard]] WindowID create_window(const std::string& title, u32 width, u32 height) override;
		void destroy_window(WindowID window) override;

		void set_window_title(WindowID window, const std::string& title) override;
		void set_window_size(WindowID window, u32 width, u32 height) override;
		void set_window_position(WindowID window, i32 x, i32 y) override;

		const char* get_surface_extension() const override;
		[[nodiscard]] SurfaceID create_surface(WindowID window, RenderDriver* driver) override;

	  private:
		X11::Display* m_display = nullptr;
		X11::Atom m_window_close_atom = 0;
		std::unordered_map<X11::Window, WindowID> m_windows;
	};
} // namespace Nova

#endif // NOVA_X11
