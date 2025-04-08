/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "drivers/dx12/render_driver.h" // IWYU pragma: keep
#include "drivers/vulkan/render_driver.h" // IWYU pragma: keep

#include <nova/core/debug.h>
#include <nova/render/render_driver.h>

using namespace Nova;

RenderDriver* RenderDriver::create(RenderAPI api, WindowDriver* window_driver) {
	NOVA_AUTO_TRACE();
	switch (api) {
#ifdef NOVA_DX12
		case RenderAPI::DX12:
			return new DX12RenderDriver();
#endif
#ifdef NOVA_VULKAN
		case RenderAPI::VULKAN:
			return new VulkanRenderDriver(window_driver);
#endif
		default:
			throw std::runtime_error("Unsupported render API");
	}
}
