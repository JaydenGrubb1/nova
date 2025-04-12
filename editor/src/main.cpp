/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <nova/core/debug.h>
#include <nova/platform/window_driver.h>
#include <nova/render/render_device.h>
#include <nova/render/render_driver.h>

#include <cstdlib>

using namespace Nova;

int main() {
	Debug::get_logger()->set_level(spdlog::level::trace);

	try {
		auto wd = WindowDriver::create();
		auto rd = RenderDriver::create(RenderAPI::VULKAN, wd);

		auto window = wd->create_window("Nova", 1280, 720);
		auto surface = rd->create_surface(window);

		auto device = RenderDevice::choose_device(rd, surface);
		rd->select_device(device);

		while (wd->get_window_count() > 0) {
			wd->poll_events();
		}

		rd->destroy_surface(surface);

		delete rd;
		delete wd;
		return EXIT_SUCCESS;
	} catch (const std::exception& e) {
		NOVA_CRITICAL("Unhandled exception: {}", e.what());
		return EXIT_FAILURE;
	}
}
