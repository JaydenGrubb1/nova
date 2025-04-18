/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#ifdef NOVA_VULKAN

#include <nova/render/render_driver.h>
#include <vulkan/vulkan.h>

#include <vector>

namespace Nova {
	struct Surface {
		VkSurfaceKHR handle = VK_NULL_HANDLE;
	};

	class VulkanRenderDriver final : public RenderDriver {
	  public:
		explicit VulkanRenderDriver(WindowDriver* window_driver);
		~VulkanRenderDriver() override;

		[[nodiscard]] RenderAPI get_api() const override;
		[[nodiscard]] u32 get_api_version() const override;
		[[nodiscard]] std::string get_api_name() const override;
		[[nodiscard]] std::string get_api_version_string() const override;

		[[nodiscard]] u32 get_device_count() const override;
		[[nodiscard]] const RenderDevice& get_device(u32 index) const override;
		[[nodiscard]] bool get_device_supports_surface(u32 index, SurfaceID surface) const override;
		void select_device(u32 index) override;

		[[nodiscard]] SurfaceID create_surface(WindowID window) override;
		void destroy_surface(SurfaceID surface) override;

		[[nodiscard]] VkInstance get_instance() const;
		[[nodiscard]] VkAllocationCallbacks* get_allocator(VkObjectType type) const;

	  private:
		WindowDriver* m_window_driver = nullptr;
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
		void _check_device_capabilities();
		void _init_queues(std::vector<VkDeviceQueueCreateInfo>& queues) const;
		void _init_device(const std::vector<VkDeviceQueueCreateInfo>& queues);
	};
} // namespace Nova

#endif // NOVA_VULKAN
