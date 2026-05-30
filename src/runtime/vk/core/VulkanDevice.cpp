#include "runtime/vk/core/VulkanDevice.hpp"

#include "core/Log.hpp"
#include "runtime/vk/core/VulkanAdapter.hpp"

#include <set>
#include <vector>

namespace RT {
	namespace {
		std::vector<const char*> requiredDeviceExtensions(const VulkanDeviceRequirements& requirements) {
			std::vector<const char*> extensions;

			if (requirements.requirePresent || requirements.enableSwapchain) {
				extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
			}

			return extensions;
		}

		bool hasRequiredQueues(
			const VulkanQueueFamilyIndices& indices,
			const VulkanDeviceRequirements& requirements
		) {
			if (requirements.requireGraphics && !indices.hasGraphics()) {
				EG_ERROR("VulkanDevice::create: missing graphics queue family");
				return false;
			}

			if (requirements.requirePresent && !indices.hasPresent()) {
				EG_ERROR("VulkanDevice::create: missing present queue family");
				return false;
			}

			if (requirements.requireCompute && !indices.hasCompute()) {
				EG_ERROR("VulkanDevice::create: missing compute queue family");
				return false;
			}

			if (requirements.requireTransfer && !indices.hasTransfer()) {
				EG_ERROR("VulkanDevice::create: missing transfer queue family");
				return false;
			}

			return true;
		}

		void addQueueFamily(std::set<uint32_t>& families, const std::optional<uint32_t>& family) {
			if (family) {
				families.insert(family.value());
			}
		}
	}

	void VulkanDevice::create(const VulkanAdapter& adapter, const VulkanDeviceRequirements& requirements) {
		if (isvalid_) {
			return;
		}

		if (!adapter.isValid()) {
			EG_ERROR("VulkanDevice::create: invalid adapter");
			return;
		}

		const VulkanQueueFamilyIndices& indices = adapter.queueFamilyIndices();
		if (!hasRequiredQueues(indices, requirements)) {
			return;
		}

		std::set<uint32_t> uniqueQueueFamilies;
		addQueueFamily(uniqueQueueFamilies, indices.graphicsFamily);
		addQueueFamily(uniqueQueueFamilies, indices.presentFamily);
		addQueueFamily(uniqueQueueFamilies, indices.computeFamily);
		addQueueFamily(uniqueQueueFamilies, indices.transferFamily);

		if (uniqueQueueFamilies.empty()) {
			EG_ERROR("VulkanDevice::create: no queue families selected");
			return;
		}

		const float queuePriority = 1.0f;
		std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
		queueCreateInfos.reserve(uniqueQueueFamilies.size());

		for (uint32_t familyIndex : uniqueQueueFamilies) {
			vk::DeviceQueueCreateInfo queueCreateInfo{};
			queueCreateInfo.setQueueFamilyIndex(familyIndex)
				.setQueueCount(1)
				.setPQueuePriorities(&queuePriority);
			queueCreateInfos.push_back(queueCreateInfo);
		}

		const std::vector<const char*> extensions = requiredDeviceExtensions(requirements);

		vk::PhysicalDeviceFeatures enabledFeatures{};
		vk::DeviceCreateInfo createInfo{};
		createInfo.setQueueCreateInfos(queueCreateInfos)
			.setPEnabledFeatures(&enabledFeatures)
			.setEnabledExtensionCount(static_cast<uint32_t>(extensions.size()))
			.setPpEnabledExtensionNames(extensions.data());

		device_ = adapter.handle().createDevice(createInfo);

		if (!device_) {
			EG_ERROR("VulkanDevice::create: failed to create logical device");
			return;
		}

		if (indices.graphicsFamily) {
			graphicsQueue_.set(device_.getQueue(indices.graphicsFamily.value(), 0), indices.graphicsFamily.value(), VulkanQueueType::Graphics);
		}

		if (indices.presentFamily) {
			presentQueue_.set(device_.getQueue(indices.presentFamily.value(), 0), indices.presentFamily.value(), VulkanQueueType::Present);
		}

		if (indices.computeFamily) {
			computeQueue_.set(device_.getQueue(indices.computeFamily.value(), 0), indices.computeFamily.value(), VulkanQueueType::Compute);
		}

		if (indices.transferFamily) {
			transferQueue_.set(device_.getQueue(indices.transferFamily.value(), 0), indices.transferFamily.value(), VulkanQueueType::Transfer);
		}

		if ((requirements.requireGraphics && !hasGraphicsQueue()) ||
			(requirements.requirePresent && !hasPresentQueue()) ||
			(requirements.requireCompute && !hasComputeQueue()) ||
			(requirements.requireTransfer && !hasTransferQueue())) {
			EG_ERROR("VulkanDevice::create: failed to retrieve required queue");
			destroy();
			return;
		}

		isvalid_ = true;

		EG_INFO("VulkanDevice::create: logical device created");

		if (hasGraphicsQueue()) {
			EG_INFO("VulkanDevice::create: graphics queue family {}", graphicsQueue_.familyIndex());
		}

		if (hasPresentQueue()) {
			EG_INFO("VulkanDevice::create: present queue family {}", presentQueue_.familyIndex());
		}

		if (hasComputeQueue()) {
			EG_INFO("VulkanDevice::create: compute queue family {}", computeQueue_.familyIndex());
		}

		if (hasTransferQueue()) {
			EG_INFO("VulkanDevice::create: transfer queue family {}", transferQueue_.familyIndex());
		}
	}
	vk::Device VulkanDevice::handle() const {
		return device_;
	}
	const VulkanQueue& VulkanDevice::graphicsQueue() const {
		return graphicsQueue_;
	}
	const VulkanQueue& VulkanDevice::presentQueue() const {
		return presentQueue_;
	}
	const VulkanQueue& VulkanDevice::computeQueue() const {
		return computeQueue_;
	}
	const VulkanQueue& VulkanDevice::transferQueue() const {
		return transferQueue_;
	}

	bool VulkanDevice::hasGraphicsQueue() const {
		return graphicsQueue_.isValid();
	}
	bool VulkanDevice::hasPresentQueue() const {
		return presentQueue_.isValid();
	}
	bool VulkanDevice::hasComputeQueue() const {
		return computeQueue_.isValid();
	}
	bool VulkanDevice::hasTransferQueue() const {
		return transferQueue_.isValid();
	}
	bool VulkanDevice::isValid() const {
		return isvalid_ && static_cast<bool>(device_);

	}
	VulkanDevice::operator bool() const {
		return isValid();
	}

	void VulkanDevice::destroy() {
		if (!isValid()) return;
		device_.waitIdle();
		graphicsQueue_.reset();
		presentQueue_.reset();
		computeQueue_.reset();
		transferQueue_.reset();
		device_.destroy();
		device_ = nullptr;
		isvalid_ = false;
	}
	VulkanDevice::~VulkanDevice() {
		destroy();
	}
}
