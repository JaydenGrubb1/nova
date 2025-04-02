/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifdef NOVA_WINDOWS

#include "drivers/win32/window_driver.h"

#ifdef NOVA_VULKAN
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_win32.h>
#endif

#include <nova/core/debug.h>

using namespace Nova;

Win32WindowDriver::Win32WindowDriver() {
	NOVA_AUTO_TRACE();
}

Win32WindowDriver::~Win32WindowDriver() {
	NOVA_AUTO_TRACE();
}

const char* Win32WindowDriver::get_surface_extension() const {
#ifdef NOVA_VULKAN
	return VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
#else
	return nullptr;
#endif
}

#endif // NOVA_WINDOWS
