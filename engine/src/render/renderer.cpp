/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <nova/render/renderer.h>

#include <memory>

#include "drivers/vulkan/render_driver.h"

using namespace Nova;

static std::unique_ptr<RenderDriver> s_driver;

void Renderer::create(const RenderAPI api) {
	switch (api) {
		case RenderAPI::VULKAN:
			s_driver = std::make_unique<VulkanRenderDriver>();
			break;
		default:
			// TODO: Log error
			break;
	}
}

void Renderer::shutdown() {
	s_driver.reset();
}

RenderDriver* Renderer::get_driver() {
	return s_driver.get();
}
