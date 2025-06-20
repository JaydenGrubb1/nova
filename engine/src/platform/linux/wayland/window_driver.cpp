/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifdef NOVA_WAYLAND

#include "platform/linux/wayland/window_driver.h"

#ifdef NOVA_VULKAN
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_wayland.h>
#endif

#include <nova/core/debug.h>

using namespace Nova;

WaylandWindowDriver::WaylandWindowDriver() {
	NOVA_AUTO_TRACE();
}

WaylandWindowDriver::~WaylandWindowDriver() {
	NOVA_AUTO_TRACE();
}

const char* WaylandWindowDriver::get_surface_extension() const {
#ifdef NOVA_VULKAN
	return VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME;
#else
	return nullptr;
#endif
}

#endif // NOVA_WAYLAND
