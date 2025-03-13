/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "drivers/vulkan/render_driver.h"

#include <nova/core/debug.h>
#include <vulkan/vulkan.h>

using namespace Nova;

VulkanRenderDriver::VulkanRenderDriver() {
	NOVA_AUTO_TRACE();
}

VulkanRenderDriver::~VulkanRenderDriver() {
	NOVA_AUTO_TRACE();
}

RenderAPI VulkanRenderDriver::get_api() const {
	return RenderAPI::VULKAN;
}

u32 VulkanRenderDriver::get_api_version() const {
	u32 version;
	vkEnumerateInstanceVersion(&version);
	return version;
}
