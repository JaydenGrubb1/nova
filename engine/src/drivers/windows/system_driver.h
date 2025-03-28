/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#include <nova/platform/system_driver.h>
#include <windows.h>

namespace Nova {
	class WindowsSystemDriver final : public SystemDriver {
	  public:
		WindowsSystemDriver();
		~WindowsSystemDriver() override;

		[[nodiscard]] const char* get_surface_extension() const override;
	};
} // namespace Nova
