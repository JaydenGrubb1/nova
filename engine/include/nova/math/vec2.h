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
	struct Vec2 {
		union {
			struct {
				T x, y;
			};
			struct {
				T u, v;
			};
			struct {
				T width, height;
			};
			T data[2];
		};
	};

	using iVec2 = Vec2<i32>;
	using uVec2 = Vec2<u32>;

	NOVA_END_ALLOW_ANONYMOUS_TYPES
} // namespace Nova
