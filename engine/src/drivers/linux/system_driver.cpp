/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "drivers/linux/system_driver.h"

#ifdef NOVA_WAYLAND
	#include "drivers/linux/wayland/system_driver.h"
#endif
#ifdef NOVA_X11
	#include "drivers/linux/x11/system_driver.h"
#endif

#include <nova/core/debug.h>

using namespace Nova;

LinuxSystemDriver::LinuxSystemDriver() {
	NOVA_AUTO_TRACE();
}

LinuxSystemDriver::~LinuxSystemDriver() {
	NOVA_AUTO_TRACE();
}

const char* LinuxSystemDriver::get_surface_extension() const {
	NOVA_WARN("System driver does not support surfaces");
	return nullptr;
}

std::unique_ptr<SystemDriver> LinuxSystemDriver::get_default_driver() {
	NOVA_AUTO_TRACE();

#ifdef NOVA_WAYLAND
	if (std::getenv("WAYLAND_DISPLAY")) {
		return std::make_unique<WaylandSystemDriver>();
	}
#endif
#ifdef NOVA_X11
	if (std::getenv("DISPLAY")) {
		return std::make_unique<X11SystemDriver>();
	}
#endif

	NOVA_WARN("Unsupported display server");
	return std::make_unique<LinuxSystemDriver>();
}
