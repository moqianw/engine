#pragma once
#include "runtime/vk/core/VulkanQueue.hpp"
#include "runtime/vk/core/utils.hpp"
#include <vulkan/vulkan.hpp>
namespace RT {
	class VulkanAdapter;

	class VulkanDevice {
	private:
		vk::Device device_ = nullptr;
		VulkanQueue graphicsQueue_{};
		VulkanQueue presentQueue_{};
		VulkanQueue computeQueue_{};
		VulkanQueue transferQueue_{};
		bool isvalid_ = false;
	public:
		VulkanDevice() = default;

		void create(const VulkanAdapter& adapter, const VulkanDeviceRequirements& requirements);
		vk::Device handle() const;
		const VulkanQueue& graphicsQueue() const;
		const VulkanQueue& presentQueue() const;
		const VulkanQueue& computeQueue() const;
		const VulkanQueue& transferQueue() const;

		bool hasGraphicsQueue() const;
		bool hasPresentQueue() const;
		bool hasComputeQueue() const;
		bool hasTransferQueue() const;
		bool isValid() const;
		explicit operator bool() const;

		void destroy();
		~VulkanDevice();

	};
}
