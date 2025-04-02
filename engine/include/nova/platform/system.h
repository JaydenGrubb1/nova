/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#include <nova/api.h>
#include <nova/platform/window_driver.h>

namespace Nova {
	class NOVA_API System {
	  public:
		static void init();
		static void shutdown();
		static WindowDriver* get_driver();
	};
} // namespace Nova
