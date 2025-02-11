/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <nova/render/renderer.h>

#include <memory>

#include "drivers/vulkan/render_driver.h"

using namespace Nova;

static Renderer* s_instance = nullptr;

void Renderer::create(const RenderAPI api) {
	if (s_instance) {
		// TODO: Log error
		return;
	}
	s_instance = new Renderer();
	switch (api) {
		case RenderAPI::VULKAN:
			get()->m_driver = std::make_unique<VulkanRenderDriver>();
			break;
		default:
			// TODO: Log error
			break;
	}
}

void Renderer::shutdown() {
	if (!s_instance) {
		// TODO: Log error
		return;
	}
	delete s_instance;
}

Renderer* Renderer::get() {
	return s_instance;
}

RenderDriver* Renderer::get_driver() {
	return get()->m_driver.get();
}
