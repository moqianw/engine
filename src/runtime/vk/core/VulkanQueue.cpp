#include "runtime/vk/core/VulkanQueue.hpp"
namespace RT {
	void VulkanQueue::set(vk::Queue q, uint32_t idx, VulkanQueueType t) {
		queue_ = q;
		familyIndex_ = idx;
		type_ = t;
	}
	void VulkanQueue::reset() {
		queue_ = nullptr;
		familyIndex_ = std::numeric_limits<uint32_t>::max();
		type_ = VulkanQueueType::UnKnown;
	}
	vk::Queue VulkanQueue::handle() const
	{
		return queue_;
	}
	uint32_t VulkanQueue::familyIndex() const {
		return familyIndex_;
	}

	VulkanQueueType VulkanQueue::type() const {
		return type_;
	}
	bool VulkanQueue::isValid() const {
		return static_cast<bool>(queue_) && familyIndex_ != std::numeric_limits<uint32_t>::max() && type_ != VulkanQueueType::UnKnown;
	}

	VulkanQueue::operator bool() const {
		return isValid();
	}
}
