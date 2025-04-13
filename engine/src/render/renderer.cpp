/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <nova/core/debug.h>
#include <nova/render/renderer.h>

#include <memory>

using namespace Nova;

static std::unique_ptr<RenderDriver> s_driver;

void Renderer::init(const RenderAPI p_api) {
	NOVA_AUTO_TRACE();
	NOVA_ASSERT(!s_driver);

	s_driver = std::unique_ptr<RenderDriver>(RenderDriver::create(p_api, nullptr));
}

void Renderer::shutdown() {
	NOVA_AUTO_TRACE();
	s_driver.reset();
}

RenderDriver* Renderer::get_driver() {
	NOVA_ASSERT(s_driver);
	return s_driver.get();
}
