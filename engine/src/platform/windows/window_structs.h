/**
* Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

/// NOTE: This header should only be included in implementation files

#include <nova/platform/platform_structs.h>
#include <windows.h>

namespace Nova {
	struct Window {
		HWND handle = {};
		int width = 0;
		int height = 0;
	};
} // namespace Nova
