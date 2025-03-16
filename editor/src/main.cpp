/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <nova/core/debug.h>
#include <nova/render/renderer.h>

#include <cstdlib>

using namespace Nova;

int main() {
	Debug::get_logger()->set_level(spdlog::level::trace);

	Renderer::create(RenderAPI::VULKAN);
	const auto driver = Renderer::get_driver();

	if (driver->get_device_count() == 0) {
		Debug::log_error("No devices found");
		return EXIT_FAILURE;
	}
	driver->create_device(0);

	Renderer::shutdown();
	return EXIT_SUCCESS;
}
