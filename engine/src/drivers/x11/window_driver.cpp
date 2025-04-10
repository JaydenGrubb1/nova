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

void X11WindowDriver::poll_events() {
	NOVA_AUTO_TRACE();

	XEvent event;
	XNextEvent(m_display, &event);

	const WindowID window = event.xany.window;
	NOVA_ASSERT(m_windows.contains(window));

	switch (event.type) {
		case Expose:
			break;
		case ClientMessage: {
			if (event.xclient.data.l[0] == static_cast<long>(m_window_close_atom)) {
				destroy_window(window);
			}
			break;
		}
		default:
			NOVA_WARN("Unhandled X11 event: {}", event.type);
			break;
	}
}

void X11WindowDriver::beep() {
	XBell(m_display, 100);
}

u32 X11WindowDriver::get_window_count() const {
	return static_cast<u32>(m_windows.size());
}

WindowID X11WindowDriver::create_window(const std::string_view title, const u32 width, const u32 height) {
	NOVA_AUTO_TRACE();

	const WindowID window = XCreateSimpleWindow(m_display, DefaultRootWindow(m_display), 0, 0, width, height, 0, 0, 0);
	const WindowData& data = m_windows[window];
	(void)data; // TODO: Initialize window data

	XSetWMProtocols(m_display, window, &m_window_close_atom, 1);
	XSelectInput(m_display, window, ExposureMask);
	XStoreName(m_display, window, title.data());
	XMapWindow(m_display, window);
	XFlush(m_display);

	return window;
}

void X11WindowDriver::destroy_window(const WindowID window) {
	NOVA_AUTO_TRACE();
	if (!m_windows.contains(window)) {
		return;
	}
	XDestroyWindow(m_display, window);
	m_windows.erase(window);
}

void X11WindowDriver::set_window_title(const WindowID window, const std::string_view title) {
	NOVA_AUTO_TRACE();
	NOVA_ASSERT(m_windows.contains(window));
	XStoreName(m_display, window, title.data());
}

void X11WindowDriver::set_window_size(const WindowID window, const u32 width, const u32 height) {
	NOVA_AUTO_TRACE();
	NOVA_ASSERT(m_windows.contains(window));
	XResizeWindow(m_display, window, width, height);
}

void X11WindowDriver::set_window_position(const WindowID window, const i32 x, const i32 y) {
	NOVA_AUTO_TRACE();
	NOVA_ASSERT(m_windows.contains(window));
	XMoveWindow(m_display, window, x, y);
}

const char* X11WindowDriver::get_surface_extension() const {
#ifdef NOVA_VULKAN
	return VK_KHR_XLIB_SURFACE_EXTENSION_NAME;
#else
	return nullptr;
#endif
}

SurfaceID X11WindowDriver::create_surface(const WindowID window, RenderDriver* render_driver) {
	NOVA_AUTO_TRACE();
	NOVA_ASSERT(m_windows.contains(window));
	NOVA_ASSERT(render_driver);
	NOVA_ASSERT(render_driver->get_api() == RenderAPI::VULKAN);

#ifdef NOVA_VULKAN
	VkXlibSurfaceCreateInfoKHR create {};
	create.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
	create.dpy = m_display;
	create.window = static_cast<Window>(window);

	const auto vkrd = static_cast<VulkanRenderDriver*>(render_driver);
	SurfaceData* surface = new SurfaceData();

	if (vkCreateXlibSurfaceKHR(vkrd->get_instance(), &create, vkrd->get_allocator(VK_OBJECT_TYPE_SURFACE_KHR), &surface->handle)
		!= VK_SUCCESS) {
		throw std::runtime_error("Failed to create Vulkan surface");
	}
	return reinterpret_cast<SurfaceID>(surface);
#else
	return SurfaceID();
#endif
}

#endif // NOVA_X11
