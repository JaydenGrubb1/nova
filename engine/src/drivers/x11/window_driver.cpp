/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifdef NOVA_X11

#include "drivers/x11/window_driver.h"

#ifdef NOVA_VULKAN
#include "drivers/vulkan/render_driver.h"

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_xlib.h>
#endif

#include <nova/core/debug.h>
#include <nova/render/render_driver.h>

#include <ranges>

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

	for (const auto window : std::views::keys(m_windows)) {
		XDestroyWindow(m_display, window);
	}

	if (m_display) {
		XCloseDisplay(m_display);
		m_display = nullptr;
	}
}

WindowAPI X11WindowDriver::get_api() const {
	return WindowAPI::X11;
}

std::string X11WindowDriver::get_api_name() const {
	return "X11";
}

void X11WindowDriver::poll_events() {
	while (XPending(m_display)) {
		XEvent event;
		XNextEvent(m_display, &event);

		const WindowID window = event.xany.window;
		WindowData& data = m_windows[window];

		switch (event.type) {
			case ConfigureNotify: {
				XConfigureEvent xce = event.xconfigure;
				if (xce.width != data.width || xce.height != data.height) {
					data.width = xce.width;
					data.height = xce.height;
					NOVA_DEBUG("Window event: RESIZED ({}x{})", xce.width, xce.height);
				}
				break;
			}
			case ClientMessage: {
				if (event.xclient.data.l[0] == static_cast<long>(m_window_close_atom)) {
					NOVA_DEBUG("Window event: CLOSED");
					destroy_window(window);
				}
				break;
			}
			case MapNotify:
			case ReparentNotify:
				// Ignore these events
				break;
			default:
				NOVA_WARN("Unhandled X11 event: {}", event.type);
				break;
		}
	}
}

void X11WindowDriver::beep() {
	XBell(m_display, 100);
}

u32 X11WindowDriver::get_window_count() const {
	return static_cast<u32>(m_windows.size());
}

WindowID X11WindowDriver::create_window(const std::string_view p_title, const u32 p_width, const u32 p_height) {
	NOVA_AUTO_TRACE();

	const WindowID window = XCreateSimpleWindow(m_display, DefaultRootWindow(m_display), 0, 0, p_width, p_height, 0, 0, 0);
	WindowData& data = m_windows[window];
	data.width = p_width;
	data.height = p_height;

	XSetWMProtocols(m_display, window, &m_window_close_atom, 1);
	XSelectInput(m_display, window, StructureNotifyMask);
	XStoreName(m_display, window, p_title.data());
	XMapWindow(m_display, window);
	XFlush(m_display);

	return window;
}

void X11WindowDriver::destroy_window(const WindowID p_window) {
	NOVA_AUTO_TRACE();
	if (!m_windows.contains(p_window)) {
		return;
	}
	XDestroyWindow(m_display, p_window);
	m_windows.erase(p_window);
}

void X11WindowDriver::set_window_title(const WindowID p_window, const std::string_view p_title) {
	NOVA_AUTO_TRACE();
	NOVA_ASSERT(m_windows.contains(p_window));
	XStoreName(m_display, p_window, p_title.data());
}

void X11WindowDriver::set_window_size(const WindowID p_window, const u32 p_width, const u32 p_height) {
	NOVA_AUTO_TRACE();
	NOVA_ASSERT(m_windows.contains(p_window));
	XResizeWindow(m_display, p_window, p_width, p_height);
}

void X11WindowDriver::set_window_position(const WindowID p_window, const i32 p_x, const i32 p_y) {
	NOVA_AUTO_TRACE();
	NOVA_ASSERT(m_windows.contains(p_window));
	XMoveWindow(m_display, p_window, p_x, p_y);
}

const char* X11WindowDriver::get_surface_extension() const {
#ifdef NOVA_VULKAN
	return VK_KHR_XLIB_SURFACE_EXTENSION_NAME;
#else
	return nullptr;
#endif
}

SurfaceID X11WindowDriver::create_surface(const WindowID p_window, RenderDriver* p_driver) {
	NOVA_AUTO_TRACE();
	NOVA_ASSERT(m_windows.contains(p_window));
	NOVA_ASSERT(p_driver);
	NOVA_ASSERT(p_driver->get_api() == RenderAPI::VULKAN);

#ifdef NOVA_VULKAN
	VkXlibSurfaceCreateInfoKHR create {};
	create.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
	create.dpy = m_display;
	create.window = static_cast<Window>(p_window);

	const auto vkrd = static_cast<VulkanRenderDriver*>(p_driver);
	SurfaceData* surface = new SurfaceData();

	if (vkCreateXlibSurfaceKHR(vkrd->get_instance(), &create, vkrd->get_allocator(VK_OBJECT_TYPE_SURFACE_KHR), &surface->handle)
		!= VK_SUCCESS) {
		throw std::runtime_error("Failed to create Vulkan surface");
	}

	return SurfaceID(surface);
#else
	return SurfaceID(nullptr);
#endif
}

#endif // NOVA_X11
