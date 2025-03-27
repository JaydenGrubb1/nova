/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <nova/core/debug.h>
#include <nova/render/render_device.h>

using namespace Nova;

u32 RenderDevice::choose_device(const std::vector<RenderDevice>& devices) {
	NOVA_AUTO_TRACE();

	u32 best_index = 0;
	u32 best_score = 0;

	for (u32 i = 0; i < devices.size(); i++) {
		u32 score = 0;
		switch (devices[i].type) {
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

	return best_index;
}
