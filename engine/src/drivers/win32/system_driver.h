/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#include <nova/platform/system_driver.h>

namespace Nova {
	class Win32SystemDriver final : public SystemDriver {
	  public:
		Win32SystemDriver();
		~Win32SystemDriver() override;
	};
} // namespace Nova
