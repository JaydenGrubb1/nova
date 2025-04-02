/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#ifdef NOVA_X11

#include <X11/Xlib.h>
#include <nova/platform/window_driver.h>

namespace Nova {
	class X11WindowDriver final : public WindowDriver {
	  public:
		X11WindowDriver();
		~X11WindowDriver() override;

		[[nodiscard]] const char* get_surface_extension() const override;
	};
} // namespace Nova

#endif // NOVA_X11
