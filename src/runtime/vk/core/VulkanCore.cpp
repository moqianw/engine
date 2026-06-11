#include "runtime/vk/core/VulkanCore.hpp"
#include "core/Log.hpp"
namespace RT {
	void VulkanCore::create() {
		if (isvalid_) return;
		instance_.create();
		if (!instance_.isValid()) {
			EG_ERROR("VulkanCore::create: failed to create Vulkan instance");
			return;
		}
		EG_INFO("VulkanCore created");
		isvalid_ = true;
	}


	void VulkanCore::createDevice(
		const VulkanDeviceRequirements& deviceRequirements,
		vk::SurfaceKHR surface
	) {
		if (!isValid()) {
			EG_ERROR("VulkanCore::createDevice: instance is not ready");
			return;
		}

		if (device_.isValid()) {
			return;
		}
		VulkanAdapterRequirements adapterRequirements{};
		adapterRequirements.requireCompute = deviceRequirements.requireCompute;
		adapterRequirements.requireGraphics = deviceRequirements.requireGraphics;
		adapterRequirements.requirePresent = deviceRequirements.requirePresent;
		adapterRequirements.requireTransfer = deviceRequirements.requireTransfer;
		if (!adapter_.isValid()) {
			adapter_.select(instance_.handle(), adapterRequirements, surface);
		}

		if (!adapter_.isValid()) {
			EG_ERROR("VulkanCore::createDevice: failed to select Vulkan adapter");
			return;
		}

		device_.create(adapter_, deviceRequirements);

		if (!device_.isValid()) {
			EG_ERROR("VulkanCore::createDevice: failed to create Vulkan device");
			return;
		}

		EG_INFO("VulkanCore device created");
	}

	void VulkanCore::destroy() {
		device_.destroy();
		adapter_.destroy();
		instance_.destroy();

		if (isvalid_) {
			EG_INFO("VulkanCore destroyed");
		}

		isvalid_ = false;
	}


	bool VulkanCore::isValid() const {
		return isvalid_ && instance_.isValid();
	}

	bool VulkanCore::hasAdapter() const {
		return adapter_.isValid();
	}

	bool VulkanCore::hasDevice() const {
		return device_.isValid();
	}

	VulkanCore::operator bool() const {
		return isValid();
	}

	const VulkanInstance& VulkanCore::vulkanInstance() const
	{
		return instance_;
	}

	VulkanInstance& VulkanCore::vulkanInstance() 
	{
		return instance_;
	}

	const VulkanAdapter& VulkanCore::vulkanAdapter() const
	{
		return adapter_;
	}

	VulkanAdapter& VulkanCore::vulkanAdapter()
	{
		return adapter_;
	}

	const VulkanDevice& VulkanCore::vulkanDevice() const
	{
		return device_;
	}

	VulkanDevice& VulkanCore::vulkanDevice()
	{
		return device_;
	}
}
