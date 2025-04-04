/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#include <nova/api.h>
#include <nova/types.h>

#include <string>
#include <vector>

namespace Nova {
	struct RenderDevice {
		static constexpr u32 AUTO = static_cast<u32>(-1);
		enum class Vendor { UNKNOWN = 0, INTEL = 0x8086, AMD = 0x1002, NVIDIA = 0x10de };
		enum class Type { OTHER = 0, INTEGRATED = 1, DISCRETE = 2, VIRTUAL = 3, CPU = 4 };

		std::string name;
		Vendor vendor;
		Type type;
		u32 deviceID;
		void* handle;

		NOVA_API static u32 choose_device(const std::vector<RenderDevice>& devices);
	};
} // namespace Nova
