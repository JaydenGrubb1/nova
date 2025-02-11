/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <nova/render/renderer.h>

#include <cstdio>
#include <cstdlib>

using namespace Nova;

int main() {
	Renderer::create(RenderAPI::VULKAN);

	const auto api = Renderer::get_driver()->get_api();
	const auto version = Renderer::get_driver()->get_api_version();

	printf("API: %d\n", static_cast<u32>(api));
	printf("Version: %d\n", version);

	Renderer::shutdown();
	return EXIT_SUCCESS;
}
