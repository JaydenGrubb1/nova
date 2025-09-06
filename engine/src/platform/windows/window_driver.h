/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#ifdef NOVA_WINDOWS

#include <nova/platform/window_driver.h>
#include <windows.h>

#include <unordered_map>

namespace Nova {
	class Win32WindowDriver final : public WindowDriver {
	  public:
		Win32WindowDriver();
		~Win32WindowDriver() override;

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

		[[nodiscard]] const char* get_surface_extension() const override;
		[[nodiscard]] SurfaceID create_surface(WindowID window, RenderDriver* p_driver) override;

	  private:
		std::unordered_map<HWND, WindowID> m_windows;

		LRESULT _handle_message(HWND handle, UINT msg, WPARAM wparam, LPARAM lparam);
		
		static LRESULT CALLBACK _window_proc(HWND handle, UINT msg, WPARAM wparam, LPARAM lparam);
	};
} // namespace Nova

#endif // NOVA_WINDOWS
