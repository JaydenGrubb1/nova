/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <nova/core/debug.h>
#include <nova/platform/system.h>

#include "drivers/win32/system_driver.h"
#include "drivers/x11/system_driver.h"

using namespace Nova;

static std::unique_ptr<SystemDriver> s_driver;

void System::init() {
	NOVA_AUTO_TRACE();
	NOVA_ASSERT(!s_driver);

#ifdef WIN32
	s_driver = std::make_unique<Win32SystemDriver>();
#else
	// TODO: Detect Wayland
	s_driver = std::make_unique<X11SystemDriver>();
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
