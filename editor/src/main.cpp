/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <nova/core/debug.h>
#include <nova/platform/system.h>
#include <nova/render/renderer.h>

#include <cstdlib>

using namespace Nova;

int main() {
	Debug::get_logger()->set_level(spdlog::level::trace);

	System::init();
	Renderer::create(RenderAPI::VULKAN);
	Renderer::get_driver()->create_device(RenderDevice::AUTO);

	Renderer::shutdown();
	System::shutdown();
	return EXIT_SUCCESS;
}
