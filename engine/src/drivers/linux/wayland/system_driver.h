/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#include "drivers/linux/system_driver.h"

namespace Nova {
class WaylandSystemDriver final : public LinuxSystemDriver {
  public:
	WaylandSystemDriver();
	~WaylandSystemDriver() override;
};
} // Nova
