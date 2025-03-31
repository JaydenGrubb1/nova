/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#ifdef NOVA_X11

#include "drivers/linux/system_driver.h"

#include <X11/Xlib.h>

namespace Nova {
	class X11SystemDriver final : public LinuxSystemDriver {
	  public:
		X11SystemDriver();
		~X11SystemDriver() override;

		[[nodiscard]] const char* get_surface_extension() const override;
	};
} // namespace Nova

#endif // NOVA_X11
