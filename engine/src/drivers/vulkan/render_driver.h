/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#include <nova/render/render_driver.h>
#include <vulkan/vulkan.h>

#include <vector>

namespace Nova {
	class VulkanRenderDriver final : public RenderDriver {
	  public:
		VulkanRenderDriver();
		~VulkanRenderDriver() override;

		[[nodiscard]] RenderAPI get_api() const override;
		[[nodiscard]] u32 get_api_version() const override;

		[[nodiscard]] u32 get_device_count() const override;
		[[nodiscard]] const RenderDevice& get_device(u32 index) const override;

	  private:
		VkInstance m_instance = VK_NULL_HANDLE;

		std::vector<const char*> m_extensions;
		std::vector<const char*> m_layers;
		std::vector<RenderDevice> m_devices;

		void _check_version() const;
		void _check_extensions();
		void _check_layers();
		void _init_instance();
		void _init_devices();

		static VkAllocationCallbacks* _get_allocator(VkObjectType type);
	};
} // namespace Nova
