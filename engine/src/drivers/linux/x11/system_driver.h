/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#include "drivers/linux/system_driver.h"

namespace Nova {
	class X11SystemDriver final : public LinuxSystemDriver {
	  public:
		X11SystemDriver();
		~X11SystemDriver() override;
	};
} // namespace Nova
