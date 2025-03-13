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
	Renderer::create(RenderAPI::VULKAN);

	const auto api = Renderer::get_driver()->get_api();
	const auto version = Renderer::get_driver()->get_api_version();

	Debug::log("API: {}", static_cast<u32>(api));
	Debug::log("Version: {}", version);

	Renderer::shutdown();
	return EXIT_SUCCESS;
}