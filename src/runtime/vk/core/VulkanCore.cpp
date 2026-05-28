#include "runtime/vk/core/VulkanCore.hpp"

namespace RT {
	void VulkanCore::create() {
		if (isvalid_) return;
		instance_.create();
		if (!instance_.isValid()) return;
		isvalid_ = true;
	}
	void VulkanCore::destroy() {
		if (!isvalid_) return;
		instance_.destroy();
		isvalid_ = false;
	}

	VulkanCore::~VulkanCore() {
		destroy();
	}

	bool VulkanCore::isValid() const {
		return isvalid_;
	}

	VulkanCore::operator bool() const {
		return isvalid_;
	}

	const VulkanInstance& VulkanCore::vulkanInstance() const
	{
		return instance_;
	}

	VulkanInstance& VulkanCore::vulkanInstance() 
	{
		return instance_;
	}
}