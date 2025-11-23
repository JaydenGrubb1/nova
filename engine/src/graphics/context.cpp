/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <nova/graphics/context.hpp>

#include <stdexcept>

#if defined(NOVA_BACKEND_VULKAN)
	#include "backends/vulkan/context.hpp"
#endif

namespace nova::gfx {

std::unique_ptr<IContext> IContext::create(API p_api) {
	switch (p_api) {
#if defined(NOVA_BACKEND_VULKAN)
		case API::VULKAN:
			return std::make_unique<VulkanContext>();
#endif
		default:
			throw std::runtime_error("Unsupported graphics API");
	}
}

} // namespace nova::gfx
