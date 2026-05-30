#pragma once

#include "runtime/vk/core/utils.hpp"
#include <vulkan/vulkan.hpp>
namespace RT {
	class VulkanAdapter {
	private:
		vk::PhysicalDevice physicalDevice_ = nullptr;
		vk::PhysicalDeviceProperties properties_{};
		vk::PhysicalDeviceFeatures features_{};
		VulkanQueueFamilyIndices queueFamilyIndices_{};
		bool isvalid_ = false;
	public:
		VulkanAdapter() = default;
		~VulkanAdapter() = default;
		void select(vk::Instance instance, const VulkanAdapterRequirements& requirements,vk::SurfaceKHR surface = nullptr);

		vk::PhysicalDevice handle() const;
		const VulkanQueueFamilyIndices& queueFamilyIndices() const;
		const vk::PhysicalDeviceProperties& properties() const;
		const vk::PhysicalDeviceFeatures& features() const;

		void destroy();
		bool isValid() const;
		explicit operator bool() const;
	};
}
