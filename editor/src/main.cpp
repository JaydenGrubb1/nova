/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <nova/core/debug.h>
#include <nova/platform/window_driver.h>
#include <nova/render/render_driver.h>

#include <cstdlib>

using namespace Nova;

int main() {
	Debug::get_logger()->set_level(spdlog::level::trace);

	auto wd = WindowDriver::create();
	auto rd = RenderDriver::create(RenderAPI::VULKAN, wd);

	rd->select_device(RenderDevice::AUTO);
	wd->create_window("Nova", 1280, 720);

	while (wd->get_window_count() > 0) {
		wd->poll_events();
	}

	delete rd;
	delete wd;
	return EXIT_SUCCESS;
}
