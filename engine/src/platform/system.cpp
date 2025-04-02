/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "drivers/wayland/window_driver.h" // IWYU pragma: keep
#include "drivers/win32/window_driver.h" // IWYU pragma: keep
#include "drivers/x11/window_driver.h" // IWYU pragma: keep

#include <nova/core/debug.h>
#include <nova/platform/system.h>

#include <memory>

using namespace Nova;

static std::unique_ptr<WindowDriver> s_driver;

void System::init() {
	NOVA_AUTO_TRACE();
	NOVA_ASSERT(!s_driver);

#ifdef NOVA_WINDOWS
	s_driver = std::make_unique<WindowsSystemDriver>();
#elif NOVA_LINUX
#ifdef NOVA_WAYLAND
	if (std::getenv("WAYLAND_DISPLAY")) {
		s_driver = std::make_unique<WaylandWindowDriver>();
		return;
	}
#endif
#ifdef NOVA_X11
	if (std::getenv("DISPLAY")) {
		s_driver = std::make_unique<X11WindowDriver>();
		return;
	}
#endif
	throw std::runtime_error("No suitable display server found");
#else
	throw std::runtime_error("Unsupported platform");
#endif
}

void System::shutdown() {
	NOVA_AUTO_TRACE();
	s_driver.reset();
}

WindowDriver* System::get_driver() {
	NOVA_ASSERT(s_driver);
	return s_driver.get();
}
