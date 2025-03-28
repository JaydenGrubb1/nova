/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "drivers/windows/system_driver.h"

#ifdef NOVA_VULKAN
	#include <vulkan/vulkan.h>
	#include <vulkan/vulkan_win32.h>
#endif

#include <nova/core/debug.h>

using namespace Nova;

WindowsSystemDriver::WindowsSystemDriver() {
	NOVA_AUTO_TRACE();
}

WindowsSystemDriver::~WindowsSystemDriver() {
	NOVA_AUTO_TRACE();
}

const char* WindowsSystemDriver::get_surface_extension() const {
#ifdef NOVA_VULKAN
	return VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
#else
	return nullptr;
#endif
}
