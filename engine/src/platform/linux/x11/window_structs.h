/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

/// NOTE: This header should only be included in implementation files

#include "platform/linux/x11/wrapper.h"

#include <nova/platform/platform_structs.h>

namespace Nova {
	struct Window {
		X11::Window handle = 0;
		int width = 0;
		int height = 0;
	};
} // namespace Nova
