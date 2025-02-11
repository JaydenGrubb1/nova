/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "drivers/vulkan/render_driver.h"

#include <vulkan/vulkan.h>

#include <cstdio>

using namespace Nova;

VulkanRenderDriver::VulkanRenderDriver() {
	std::printf("VulkanRenderDriver::VulkanRenderDriver()\n");
}

VulkanRenderDriver::~VulkanRenderDriver() {
	std::printf("VulkanRenderDriver::~VulkanRenderDriver()\n");
}

RenderAPI VulkanRenderDriver::get_api() const {
	return RenderAPI::VULKAN;
}

u32 VulkanRenderDriver::get_api_version() const {
	u32 version;
	vkEnumerateInstanceVersion(&version);
	return version;
}
