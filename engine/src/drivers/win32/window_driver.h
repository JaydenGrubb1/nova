/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#ifdef NOVA_WINDOWS

#include <nova/platform/window_driver.h>
#include <windows.h>

namespace Nova {
	class Win32WindowDriver final : public WindowDriver {
	  public:
		Win32WindowDriver();
		~Win32WindowDriver() override;

		[[nodiscard]] const char* get_surface_extension() const override;
	};
} // namespace Nova

#endif // NOVA_WINDOWS
