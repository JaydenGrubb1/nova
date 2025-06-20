/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#ifdef NOVA_WAYLAND

#include <nova/platform/window_driver.h>

namespace Nova {
	class WaylandWindowDriver final : public WindowDriver {
	  public:
		WaylandWindowDriver();
		~WaylandWindowDriver() override;

		[[nodiscard]] const char* get_surface_extension() const override;
	};
} // namespace Nova

#endif // NOVA_WAYLAND
