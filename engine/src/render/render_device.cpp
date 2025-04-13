/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <nova/core/debug.h>
#include <nova/render/render_device.h>
#include <nova/render/render_driver.h>

using namespace Nova;

u32 RenderDevice::choose_device(RenderDriver* p_driver, std::span<const SurfaceID> p_surfaces) {
	NOVA_AUTO_TRACE();

	u32 best_index = -1;
	u32 best_score = 0;

	for (u32 i = 0; i < p_driver->get_device_count(); i++) {
		auto& device = p_driver->get_device(i);
		u32 score = 1;

		for (SurfaceID surface : p_surfaces) {
			if (!p_driver->get_device_supports_surface(i, surface)) {
				score = 0;
				break;
			}
		}
		if (score == 0) {
			continue;
		}

		switch (device.type) {
			case Type::DISCRETE:
				score += 4;
				break;
			case Type::INTEGRATED:
				score += 3;
				break;
			case Type::VIRTUAL:
				score += 2;
				break;
			case Type::CPU:
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

	if (best_index == -1U) {
		throw std::runtime_error("No suitable render device found");
	}

	return best_index;
}
