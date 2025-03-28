/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#include <nova/api.h>
#include <nova/platform/system_driver.h>

namespace Nova {
	class NOVA_API System {
	  public:
		static void init();
		static void shutdown();
		static SystemDriver* get_driver();
	};
} // namespace Nova
