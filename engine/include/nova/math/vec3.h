/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#include <nova/pragma.h>
#include <nova/types.h>

namespace Nova {
	NOVA_BEGIN_ALLOW_ANONYMOUS_TYPES

	template<typename T = f32>
	struct Vec3 {
		union {
			struct {
				T x, y, z;
			};
			struct {
				T r, g, b;
			};
			T data[3];
		};
	};

	using iVec3 = Vec3<i32>;
	using uVec3 = Vec3<u32>;

	NOVA_END_ALLOW_ANONYMOUS_TYPES
} // namespace Nova
