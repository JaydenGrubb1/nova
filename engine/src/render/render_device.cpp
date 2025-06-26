/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <nova/core/debug.h>
#include <nova/render/render_device.h>
#include <nova/render/render_driver.h>

#include <algorithm>
#include <limits>

using namespace Nova;

u32 RenderDevice::choose_device(RenderDriver* p_driver, std::span<const SurfaceID> p_surfaces) {
	NOVA_AUTO_TRACE();

	u32 best_index = std::numeric_limits<u32>::max();
	u32 best_score = 0;

	const bool prefer_discrete = true; // TODO: Get from config

	for (u32 i = 0; i < p_driver->get_device_count(); i++) {
		auto& device = p_driver->get_device(i);
		u32 score = 1;

		if (!std::all_of(p_surfaces.begin(), p_surfaces.end(), [&](SurfaceID surface) {
				return p_driver->get_device_supports_surface(i, surface);
			})) {
			continue;
		}

		switch (device.type) {
			case DeviceType::DISCRETE:
				score += prefer_discrete ? 4 : 3;
				break;
			case DeviceType::INTEGRATED:
				score += prefer_discrete ? 3 : 4;
				break;
			case DeviceType::VIRTUAL:
				score += 2;
				break;
			case DeviceType::CPU:
				score += 1;
				break;
			default:
				break;
		}

		if (score > best_score) {
			best_index = i;
			best_score = score;
		}
	}

	if (best_index == std::numeric_limits<u32>::max()) {
		throw std::runtime_error("No suitable render device found");
	}

	return best_index;
}
