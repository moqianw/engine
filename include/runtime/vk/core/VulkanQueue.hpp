#pragma once
#include "runtime/vk/core/types.hpp"
#include <vulkan/vulkan.hpp>
#include <limits>
namespace RT {
	class VulkanQueue {
	private:
		vk::Queue queue_ = nullptr;
		uint32_t familyIndex_ = std::numeric_limits<uint32_t>::max();
		VulkanQueueType type_ = VulkanQueueType::UnKnown;
	public:
		VulkanQueue() = default;
		void set(vk::Queue queue, uint32_t familyindex, VulkanQueueType queuetype);
		void reset();

		vk::Queue handle() const;
		uint32_t familyIndex() const;
		VulkanQueueType type() const;

		bool isValid() const;
		explicit operator bool() const;
	};
}