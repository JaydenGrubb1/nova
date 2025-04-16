/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

namespace Nova {
	enum class WindowAPI { WAYLAND, WINDOWS, X11 };

	class WindowDriver;

	struct Window;
	using WindowID = Window*;
} // namespace Nova
