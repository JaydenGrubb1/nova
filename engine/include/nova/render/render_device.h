/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#include <nova/api.h>
#include <nova/render/render_structs.h>
#include <nova/types.h>

#include <initializer_list>
#include <span>
#include <string>

namespace Nova {
	class RenderDriver;

	enum class DeviceVendor { UNKNOWN = 0, INTEL = 0x8086, AMD = 0x1002, NVIDIA = 0x10de };
	enum class DeviceType { OTHER = 0, INTEGRATED = 1, DISCRETE = 2, VIRTUAL = 3, CPU = 4 };

	struct NOVA_API RenderDevice {
		std::string name;
		DeviceVendor vendor;
		DeviceType type;
		u32 deviceID;
		void* handle;

		static u32 choose_device(RenderDriver* driver, std::span<const SurfaceID> surfaces = {});
		static u32 choose_device(RenderDriver* driver, std::initializer_list<const SurfaceID> surfaces) {
			return choose_device(driver, {surfaces.begin(), surfaces.end()});
		}
		static u32 choose_device(RenderDriver* driver, SurfaceID surface) {
			return choose_device(driver, {surface});
		}
	};
} // namespace Nova
