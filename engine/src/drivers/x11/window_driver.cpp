/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifdef NOVA_X11

#include "drivers/x11/window_driver.h"

#ifdef NOVA_VULKAN
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_xlib.h>
#endif

#include <nova/core/debug.h>

using namespace Nova;

X11WindowDriver::X11WindowDriver() {
	NOVA_AUTO_TRACE();

	m_display = XOpenDisplay(nullptr);
	if (!m_display) {
		throw std::runtime_error("Failed to connect to X server");
	}

	m_window_close_atom = XInternAtom(m_display, "WM_DELETE_WINDOW", False);
}

X11WindowDriver::~X11WindowDriver() {
	NOVA_AUTO_TRACE();

	for (const auto& [id, _] : m_windows) {
		XDestroyWindow(m_display, id);
	}

	if (m_display) {
		XCloseDisplay(m_display);
		m_display = nullptr;
	}
}

void X11WindowDriver::poll_events() {
	NOVA_AUTO_TRACE();

	XEvent event;
	XNextEvent(m_display, &event);

	WindowID id = event.xany.window;
	NOVA_ASSERT(m_windows.contains(id));

	switch (event.type) {
		case Expose:
			break;
		case ClientMessage: {
			if (event.xclient.data.l[0] == static_cast<long>(m_window_close_atom)) {
				destroy_window(id);
			}
			break;
		}
		default:
			NOVA_WARN("Unhandled X11 event: {}", event.type);
			break;
	}
}

WindowID X11WindowDriver::create_window(std::string_view title, u32 width, u32 height) {
	NOVA_AUTO_TRACE();

	WindowID window = XCreateSimpleWindow(m_display, DefaultRootWindow(m_display), 0, 0, width, height, 0, 0, 0);
	WindowData& data = m_windows[window];
	(void)data; // TODO: Initialize window data

	XSetWMProtocols(m_display, window, &m_window_close_atom, 1);
	XSelectInput(m_display, window, ExposureMask);
	XStoreName(m_display, window, title.data());
	XMapWindow(m_display, window);
	XFlush(m_display);

	return window;
}

void X11WindowDriver::destroy_window(WindowID id) {
	NOVA_AUTO_TRACE();
	if (!m_windows.contains(id)) {
		return;
	}
	XDestroyWindow(m_display, id);
	m_windows.erase(id);
}

void X11WindowDriver::set_window_title(WindowID id, std::string_view title) {
	NOVA_AUTO_TRACE();
	NOVA_ASSERT(m_windows.contains(id));
	XStoreName(m_display, id, title.data());
}

void X11WindowDriver::set_window_size(WindowID id, u32 width, u32 height) {
	NOVA_AUTO_TRACE();
	NOVA_ASSERT(m_windows.contains(id));
	XResizeWindow(m_display, id, width, height);
}

void X11WindowDriver::set_window_position(WindowID id, i32 x, i32 y) {
	NOVA_AUTO_TRACE();
	NOVA_ASSERT(m_windows.contains(id));
	XMoveWindow(m_display, id, x, y);
}

u32 X11WindowDriver::get_window_count() const {
	return m_windows.size();
}

const char* X11WindowDriver::get_surface_extension() const {
#ifdef NOVA_VULKAN
	return VK_KHR_XLIB_SURFACE_EXTENSION_NAME;
#else
	return nullptr;
#endif
}

#endif // NOVA_X11
