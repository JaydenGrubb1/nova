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
	struct Shader {
		VkShaderModule handle = VK_NULL_HANDLE;
	};

	struct Surface {
		VkSurfaceKHR handle = VK_NULL_HANDLE;
		u32 width = 0;
		u32 height = 0;
		bool dirty = false; // TODO: Use state enum
	};

	struct Swapchain {
		VkSwapchainKHR handle = VK_NULL_HANDLE;
		VkFormat format = VK_FORMAT_UNDEFINED;
		VkColorSpaceKHR color_space = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
		std::vector<VkImage> images;
		std::vector<VkImageView> image_views;
		std::vector<VkFramebuffer> framebuffers;
		SurfaceID surface = nullptr;
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

		[[nodiscard]] SwapchainID create_swapchain(SurfaceID surface) override;
		void resize_swapchain(SwapchainID swapchain) override;
		void destroy_swapchain(SwapchainID swapchain) override;

		[[nodiscard]] ShaderID create_shader(const std::span<u8> bytes) override;
		void destroy_shader(ShaderID shader) override;

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
