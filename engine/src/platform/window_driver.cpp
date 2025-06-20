/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "platform/linux/wayland/window_driver.h" // IWYU pragma: keep
#include "platform/linux/x11/window_driver.h" // IWYU pragma: keep
#include "platform/windows/window_driver.h" // IWYU pragma: keep

#include <nova/core/debug.h>
#include <nova/platform/window_driver.h>

using namespace Nova;

WindowDriver* WindowDriver::create() {
	NOVA_AUTO_TRACE();
#ifdef NOVA_WINDOWS
	return new Win32WindowDriver();
#elif NOVA_LINUX
#ifdef NOVA_WAYLAND
	if (std::getenv("WAYLAND_DISPLAY")) {
		return new WaylandWindowDriver();
	}
#endif
#ifdef NOVA_X11
	if (std::getenv("DISPLAY")) {
		return new X11WindowDriver();
	}
#endif
	throw std::runtime_error("No suitable display server found");
#else
	throw std::runtime_error("Unsupported platform");
#endif
}
