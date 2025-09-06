/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifdef NOVA_WINDOWS

#include "platform/windows/window_driver.h"

#include "platform/windows/window_structs.h"

#ifdef NOVA_VULKAN
#include "drivers/vulkan/render_driver.h"
#include "drivers/vulkan/render_structs.h"

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_win32.h>
#endif

#include <nova/core/debug.h>
#include <nova/render/render_driver.h>

namespace {
	static constexpr LPCSTR WINDOW_CLASS_NAME = "NOVA_WindowClass";
}

using namespace Nova;

Win32WindowDriver::Win32WindowDriver() {
	NOVA_AUTO_TRACE();

	WNDCLASSEX wc = {};
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = _window_proc;
	wc.hInstance = GetModuleHandle(nullptr);
	wc.lpszClassName = WINDOW_CLASS_NAME;

	if (!RegisterClassEx(&wc)) {
		throw std::runtime_error("Failed to register window class");
	}
}

Win32WindowDriver::~Win32WindowDriver() {
	NOVA_AUTO_TRACE();
	UnregisterClass(WINDOW_CLASS_NAME, GetModuleHandle(nullptr));
}

WindowAPI Win32WindowDriver::get_api() const {
	return WindowAPI::WINDOWS;
}

std::string Win32WindowDriver::get_api_name() const {
	return "Win32";
}

void Win32WindowDriver::poll_events() {
	MSG msg;
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void Win32WindowDriver::beep() {
	MessageBeep(MB_OK);
}

u32 Win32WindowDriver::get_window_count() const {
	return static_cast<u32>(m_windows.size());
}

WindowID Win32WindowDriver::create_window(const std::string& p_title, u32 p_width, u32 p_height) {
	NOVA_AUTO_TRACE();

	RECT rect = {0, 0, static_cast<LONG>(p_width), static_cast<LONG>(p_height)};
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

	HWND handle = CreateWindowEx(
		0,
		WINDOW_CLASS_NAME,
		p_title.c_str(),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		rect.right - rect.left,
		rect.bottom - rect.top,
		nullptr,
		nullptr,
		GetModuleHandle(nullptr),
		this
	);

	Window* window = new Window();
	window->width = p_width;
	window->height = p_height;
	window->handle = handle;

	ShowWindow(handle, SW_SHOW);
	UpdateWindow(handle);

	m_windows[handle] = window;
	return window;
}

void Win32WindowDriver::destroy_window(WindowID p_window) {
	NOVA_AUTO_TRACE();
	NOVA_ASSERT(p_window);
	DestroyWindow(p_window->handle);
	m_windows.erase(p_window->handle);
}

void Win32WindowDriver::set_window_title(WindowID p_window, const std::string& p_title) {
	NOVA_AUTO_TRACE();
	NOVA_ASSERT(p_window);
	SetWindowText(p_window->handle, p_title.c_str());
}

void Win32WindowDriver::set_window_size(WindowID p_window, u32 p_width, u32 p_height) {
	NOVA_AUTO_TRACE();
	NOVA_ASSERT(p_window);
	RECT rect = {0, 0, static_cast<LONG>(p_width), static_cast<LONG>(p_height)};
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
	SetWindowPos(p_window->handle, nullptr, 0, 0, rect.right - rect.left, rect.bottom - rect.top, SWP_NOZORDER | SWP_NOMOVE);
}

void Win32WindowDriver::set_window_position(WindowID p_window, i32 p_x, i32 p_y) {
	NOVA_AUTO_TRACE();
	NOVA_ASSERT(p_window);
	SetWindowPos(p_window->handle, nullptr, p_x, p_y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
}

const char* Win32WindowDriver::get_surface_extension() const {
#ifdef NOVA_VULKAN
	return VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
#else
	return nullptr;
#endif
}

SurfaceID Win32WindowDriver::create_surface(WindowID p_window, RenderDriver* p_driver) {
	NOVA_AUTO_TRACE();
	NOVA_ASSERT(p_window);
	NOVA_ASSERT(p_driver);
	NOVA_ASSERT(p_driver->get_api() == RenderAPI::VULKAN);

#ifdef NOVA_VULKAN
	VkWin32SurfaceCreateInfoKHR create {};
	create.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	create.hinstance = GetModuleHandle(nullptr);
	create.hwnd = p_window->handle;

	const auto vkrd = static_cast<VulkanRenderDriver*>(p_driver);
	Surface* surface = new Surface();

	if (vkCreateWin32SurfaceKHR(vkrd->get_instance(), &create, vkrd->get_allocator(VK_OBJECT_TYPE_SURFACE_KHR), &surface->handle)
		!= VK_SUCCESS) {
		throw std::runtime_error("Failed to create Vulkan surface");
	}

	return surface;
#else
	return nullptr;
#endif
}

LRESULT Win32WindowDriver::_handle_message(HWND p_handle, UINT p_msg, WPARAM p_wparam, LPARAM p_lparam) {
	auto iter = m_windows.find(p_handle);
	if (iter == m_windows.end()) {
		return DefWindowProc(p_handle, p_msg, p_wparam, p_lparam);
	}

	WindowID window = iter->second;

	switch (p_msg) {
		case WM_DESTROY: {
			NOVA_DEBUG("Window event: DESTROYED");
			if (m_windows.empty()) {
				PostQuitMessage(0);
			}
			return 0;
		}
		case WM_SIZE: {
			int width = LOWORD(p_lparam);
			int height = HIWORD(p_lparam);
			if (width != window->width || height != window->height) {
				window->width = width;
				window->height = height;
				NOVA_DEBUG("Window event: RESIZED ({}x{})", width, height);
			}
			return 0;
		}
		case WM_CLOSE: {
			NOVA_DEBUG("Window event: CLOSED");
			destroy_window(window);
			return 0;
		}
		default:
			return DefWindowProc(p_handle, p_msg, p_wparam, p_lparam);
	}
}

LRESULT CALLBACK Win32WindowDriver::_window_proc(HWND p_handle, UINT p_msg, WPARAM p_wparam, LPARAM p_lparam) {
	Win32WindowDriver* driver = nullptr;

	if (p_msg == WM_NCCREATE) {
		CREATESTRUCT* cs = reinterpret_cast<CREATESTRUCT*>(p_lparam);
		driver = static_cast<Win32WindowDriver*>(cs->lpCreateParams);
		SetWindowLongPtr(p_handle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(driver));
	} else {
		driver = reinterpret_cast<Win32WindowDriver*>(GetWindowLongPtr(p_handle, GWLP_USERDATA));
	}

	if (driver) {
		return driver->_handle_message(p_handle, p_msg, p_wparam, p_lparam);
	}

	return DefWindowProc(p_handle, p_msg, p_wparam, p_lparam);
}

#endif // NOVA_WINDOWS
