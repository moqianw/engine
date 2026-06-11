#include "runtime/vk/VulkanBackend.hpp"
#include "runtime/platform/Qt/QtWindow.hpp"

#include "core/Log.hpp"
namespace RT {
	void VulkanBackend::create() {
		if (isvalid_) return;
		core_.create();
		if (!core_.isValid()) return;
		EG_INFO("VulkanBaclend created");
		isvalid_ = true;
	}
	void VulkanBackend::destroy() {
		if (!isvalid_) return;
		core_.destroy();
		EG_INFO("VulkanBackend destroyed");
		isvalid_ = false;
	}

	bool VulkanBackend::isValid() const {
		return isvalid_;
	}

	VulkanBackend::operator bool() const {
		return isvalid_;
	}

}