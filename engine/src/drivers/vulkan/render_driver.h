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
		[[nodiscard]] std::string get_api_name() const override;
		[[nodiscard]] std::string get_api_version_string() const override;

		[[nodiscard]] u32 get_device_count() const override;
		[[nodiscard]] const RenderDevice& get_device(u32 index) const override;
		void create_device(u32 index) override;

	  private:
		VkInstance m_instance = VK_NULL_HANDLE;
		VkPhysicalDevice m_physical_device = VK_NULL_HANDLE;
		VkDevice m_device = VK_NULL_HANDLE;
		VkPhysicalDeviceFeatures m_features = {};

		std::vector<const char*> m_extensions;
		std::vector<const char*> m_layers;
		std::vector<const char*> m_device_extensions;
		std::vector<RenderDevice> m_devices;

		void _check_version() const;
		void _check_extensions();
		void _check_layers();
		void _init_instance();
		void _init_hardware();

		void _check_device_extensions();
		void _check_device_features();
		void _init_queues(std::vector<VkDeviceQueueCreateInfo>& queues) const;
		void _init_device(const std::vector<VkDeviceQueueCreateInfo>& queues);

		static VkAllocationCallbacks* _get_allocator(VkObjectType type);
	};
} // namespace Nova
