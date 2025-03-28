/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#include <nova/platform/system_driver.h>

#include <memory>

namespace Nova {
	class LinuxSystemDriver : public SystemDriver {
	  public:
		LinuxSystemDriver();
		~LinuxSystemDriver() override;

		[[nodiscard]] const char* get_surface_extension() const override;

		static std::unique_ptr<SystemDriver> get_default_driver();
	};
} // namespace Nova
