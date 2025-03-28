/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#include <nova/api.h>

namespace Nova {
	class NOVA_API SystemDriver {
	  public:
		virtual ~SystemDriver() = default;

		[[nodiscard]] virtual const char* get_surface_extension() const = 0;
	};
} // namespace Nova
