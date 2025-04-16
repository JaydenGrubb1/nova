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
	struct Vec4 {
		union {
			struct {
				T x, y, z, w;
			};
			struct {
				T r, g, b, a;
			};
			T data[4];
		};
	};

	using iVec4 = Vec4<i32>;
	using uVec4 = Vec4<u32>;

	NOVA_END_ALLOW_ANONYMOUS_TYPES
} // namespace Nova
