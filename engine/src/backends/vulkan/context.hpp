/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#include <nova/graphics/context.hpp>

#include <vulkan/vulkan.h>

namespace nova::gfx {

class VulkanContext final : public IContext {
  public:
	API get_api() const override;
	std::string get_api_name() const override;

	u32 get_api_version() const override;
	std::string get_api_version_string() const override;

	std::unique_ptr<IDevice> create_device() override;

  private:
	VkInstance m_instance = VK_NULL_HANDLE;
};

} // namespace nova::gfx
