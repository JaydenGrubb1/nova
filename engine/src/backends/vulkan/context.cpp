/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#if defined(NOVA_USE_VOLK)
	#include <volk.h>
#endif

#include <vulkan/vulkan.h>

#include <format>

#include "backends/vulkan/context.hpp"
#include "backends/vulkan/device.hpp"

namespace nova::gfx {

VulkanContext::VulkanContext() {
#if defined(NOVA_USE_VOLK)
	if (volkInitialize() != VK_SUCCESS) {
		throw std::runtime_error("Failed to initialize volk");
	}
#endif
}

API VulkanContext::get_api() const {
	return API::VULKAN;
}

std::string VulkanContext::get_api_name() const {
	return "Vulkan";
}

u32 VulkanContext::get_api_version() const {
	u32 version;
	vkEnumerateInstanceVersion(&version);
	return version;
}

std::string VulkanContext::get_api_version_string() const {
	const u32 version = get_api_version();
	return std::format(
		"{}.{}.{}-{}",
		VK_API_VERSION_MAJOR(version),
		VK_API_VERSION_MINOR(version),
		VK_API_VERSION_PATCH(version),
		VK_API_VERSION_VARIANT(version)
	);
}

std::unique_ptr<IDevice> VulkanContext::create_device() {
	return std::make_unique<VulkanDevice>();
}

} // namespace nova::gfx
