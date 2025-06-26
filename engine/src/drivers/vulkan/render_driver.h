/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#ifdef NOVA_VULKAN

#include <nova/render/render_driver.h>

#include <vulkan/vulkan.h>

#include <unordered_map>
#include <vector>

namespace Nova {
	class VulkanRenderDriver final : public RenderDriver {
	  public:
		explicit VulkanRenderDriver(WindowDriver* window_driver);
		~VulkanRenderDriver() override;

		RenderAPI get_api() const override;
		u32 get_api_version() const override;
		std::string get_api_name() const override;
		std::string get_api_version_string() const override;

		u32 get_device_count() const override;
		const RenderDevice& get_device(u32 index) const override;
		bool get_device_supports_surface(u32 index, SurfaceID surface) const override;
		void select_device(u32 index) override;

		u32 choose_queue_family(QueueType type, SurfaceID surface) override;

		[[nodiscard]] QueueID get_queue(u32 queue_family) override;
		void free_queue(QueueID queue) override;

		[[nodiscard]] SurfaceID create_surface(WindowID window) override;
		void destroy_surface(SurfaceID surface) override;

		[[nodiscard]] SwapchainID create_swapchain(SurfaceID surface) override;
		void resize_swapchain(SwapchainID swapchain) override;
		RenderPassID get_swapchain_render_pass(SwapchainID swapchain) const override;
		void destroy_swapchain(SwapchainID swapchain) override;

		[[nodiscard]] ShaderID create_shader(const std::span<u8> bytes, ShaderStage stage) override;
		void destroy_shader(ShaderID shader) override;

		[[nodiscard]] RenderPassID create_render_pass(RenderPassParams& params) override;
		void destroy_render_pass(RenderPassID render_pass) override;

		[[nodiscard]] PipelineID create_pipeline(GraphicsPipelineParams& params) override;
		[[nodiscard]] PipelineID create_pipeline(ComputePipelineParams& params) override;
		void destroy_pipeline(PipelineID pipeline) override;

		[[nodiscard]] CommandPoolID create_command_pool(QueueID queue) override;
		void destroy_command_pool(CommandPoolID command_pool) override;

		[[nodiscard]] CommandBufferID create_command_buffer(CommandPoolID pool) override;
		void begin_command_buffer(CommandBufferID command_buffer) override;
		void end_command_buffer(CommandBufferID command_buffer) override;

		VkInstance get_instance() const;
		VkAllocationCallbacks* get_allocator(VkObjectType type) const;

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
		std::vector<Queue> m_queues;
		std::unordered_map<u32, VkQueueFlags> m_queue_families;

		void _check_version() const;
		void _check_extensions();
		void _check_layers();
		void _init_instance();
		void _init_hardware();

		void _check_device_extensions();
		void _check_device_features();
		void _check_device_capabilities();
		void _init_queues(std::vector<VkDeviceQueueCreateInfo>& queues);
		void _init_device(const std::vector<VkDeviceQueueCreateInfo>& queues);
	};
} // namespace Nova

#endif // NOVA_VULKAN
