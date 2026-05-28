#include "runtime/vk/VulkanBackend.hpp"
#include "runtime/platform/Qt/QtWindow.hpp"
namespace RT {
	void VulkanBackend::create() {
		if (isvalid_) return;
		core_.create();
		if (!core_.isValid()) return;
		isvalid_ = true;
	}
	void VulkanBackend::destroy() {
		if (!isvalid_) return;
		core_.destroy();

		//test
		surface_.destroy();
		isvalid_ = false;
	}

	VulkanBackend::~VulkanBackend() {
		destroy();
	}

	bool VulkanBackend::isValid() const {
		return isvalid_;
	}

	VulkanBackend::operator bool() const {
		return isvalid_;
	}

	void VulkanBackend::pushtestsurface(PL::QtWindow* window)
	{
		if (!window) return;
		surface_.create(core_.vulkanInstance().handle(), window->nativeHandle());
		if (!surface_.isValid()) return;
	}
}