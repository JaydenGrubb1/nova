/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifdef NOVA_WAYLAND

#include "drivers/linux/wayland/system_driver.h"

#include <nova/core/debug.h>

using namespace Nova;

WaylandSystemDriver::WaylandSystemDriver() {
	NOVA_AUTO_TRACE();
}

WaylandSystemDriver::~WaylandSystemDriver() {
	NOVA_AUTO_TRACE();
}

#endif // NOVA_WAYLAND
