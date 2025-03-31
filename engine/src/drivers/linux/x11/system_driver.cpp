/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifdef NOVA_X11

#include "drivers/linux/x11/system_driver.h"

#ifdef NOVA_VULKAN
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_xlib.h>
#endif

#include <nova/core/debug.h>

using namespace Nova;

X11SystemDriver::X11SystemDriver() {
	NOVA_AUTO_TRACE();
}

X11SystemDriver::~X11SystemDriver() {
	NOVA_AUTO_TRACE();
}

const char* X11SystemDriver::get_surface_extension() const {
#ifdef NOVA_VULKAN
	return VK_KHR_XLIB_SURFACE_EXTENSION_NAME;
#else
	return nullptr;
#endif
}

#endif // NOVA_X11
