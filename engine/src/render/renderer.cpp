/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "drivers/dx12/render_driver.h" // IWYU pragma: keep
#include "drivers/vulkan/render_driver.h" // IWYU pragma: keep

#include <nova/core/debug.h>
#include <nova/render/renderer.h>

#include <memory>

using namespace Nova;

static std::unique_ptr<RenderDriver> s_driver;

void Renderer::init(const RenderAPI api) {
	NOVA_AUTO_TRACE();
	NOVA_ASSERT(!s_driver);

	switch (api) {
#ifdef NOVA_DX12
		case RenderAPI::DX12:
			s_driver = std::make_unique<DX12RenderDriver>();
			break;
#endif
#ifdef NOVA_VULKAN
		case RenderAPI::VULKAN:
			s_driver = std::make_unique<VulkanRenderDriver>();
			break;
#endif
		default:
			throw std::runtime_error("Unsupported render API");
	}
}

void Renderer::shutdown() {
	NOVA_AUTO_TRACE();
	s_driver.reset();
}

RenderDriver* Renderer::get_driver() {
	NOVA_ASSERT(s_driver);
	return s_driver.get();
}
