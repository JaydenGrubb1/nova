/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#include <nova/platform/system_driver.h>

namespace Nova {
	class LinuxSystemDriver : public SystemDriver {
	  public:
		LinuxSystemDriver();
		~LinuxSystemDriver() override;
	};
} // namespace Nova
