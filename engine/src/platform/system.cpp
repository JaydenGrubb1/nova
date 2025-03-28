/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifdef NOVA_WINDOWS
	#include "drivers/windows/system_driver.h"
#endif
#ifdef NOVA_LINUX
	#include "drivers/linux/system_driver.h"
#endif

#include <nova/core/debug.h>
#include <nova/platform/system.h>

#include <memory>

using namespace Nova;

static std::unique_ptr<SystemDriver> s_driver;

void System::init() {
	NOVA_AUTO_TRACE();
	NOVA_ASSERT(!s_driver);

#ifdef NOVA_WINDOWS
	s_driver = std::make_unique<WindowsSystemDriver>();
#elif NOVA_LINUX
	s_driver = LinuxSystemDriver::get_default_driver();
#else
	throw std::runtime_error("Unsupported platform");
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
