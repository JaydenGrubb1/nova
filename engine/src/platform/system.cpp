/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <nova/core/debug.h>
#include <nova/platform/system.h>

#include "drivers/linux/wayland/system_driver.h"
#include "drivers/linux/x11/system_driver.h"
#include "drivers/windows/system_driver.h"

using namespace Nova;

static std::unique_ptr<SystemDriver> s_driver;

void System::init() {
	NOVA_AUTO_TRACE();
	NOVA_ASSERT(!s_driver);

#ifdef WIN32
	s_driver = std::make_unique<WindowsSystemDriver>();
#else
	if (std::getenv("WAYLAND_DISPLAY")) {
		s_driver = std::make_unique<WaylandSystemDriver>();
	} else if (std::getenv("DISPLAY")) {
		s_driver = std::make_unique<X11SystemDriver>();
	} else {
		NOVA_ERROR("Unsupported windowing system");
		s_driver = std::make_unique<LinuxSystemDriver>();
	}
#endif
}

void System::shutdown() {
	NOVA_AUTO_TRACE();
	s_driver.reset();
}

SystemDriver* System::get_driver() {
	NOVA_ASSERT(s_driver);
	return s_driver.get();
}
