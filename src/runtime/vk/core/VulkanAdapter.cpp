#include "runtime/vk/core/VulkanAdapter.hpp"

#include "core/Log.hpp"

#include <limits>
#include <set>
#include <string>
#include <vector>

namespace RT {
	namespace {
		std::vector<const char*> requiredDeviceExtensions(const VulkanAdapterRequirements& requirements) {
			std::vector<const char*> extensions;

			if (requirements.requirePresent) {
				extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
			}

			return extensions;
		}

		const char* deviceTypeName(vk::PhysicalDeviceType type) {
			switch (type) {
			case vk::PhysicalDeviceType::eDiscreteGpu:
				return "Discrete GPU";
			case vk::PhysicalDeviceType::eIntegratedGpu:
				return "Integrated GPU";
			case vk::PhysicalDeviceType::eVirtualGpu:
				return "Virtual GPU";
			case vk::PhysicalDeviceType::eCpu:
				return "CPU";
			default:
				return "Other";
			}
		}

		std::string physicalDeviceName(const vk::PhysicalDeviceProperties& properties) {
			return std::string(properties.deviceName.data());
		}

		bool hasDeviceExtensions(
			vk::PhysicalDevice device,
			const VulkanAdapterRequirements& requirements
		) {
			std::set<std::string> missingExtensions;

			for (const char* extension : requiredDeviceExtensions(requirements)) {
				missingExtensions.insert(extension);
			}

			if (missingExtensions.empty()) {
				return true;
			}

			for (const vk::ExtensionProperties& extension : device.enumerateDeviceExtensionProperties()) {
				missingExtensions.erase(extension.extensionName.data());
			}

			return missingExtensions.empty();
		}

		VulkanQueueFamilyIndices findQueueFamilies(
			vk::PhysicalDevice device,
			const VulkanAdapterRequirements& requirements,
			vk::SurfaceKHR surface
		) {
			VulkanQueueFamilyIndices indices{};
			const std::vector<vk::QueueFamilyProperties> queueFamilies = device.getQueueFamilyProperties();

			for (uint32_t familyIndex = 0; familyIndex < queueFamilies.size(); ++familyIndex) {
				const vk::QueueFamilyProperties& queueFamily = queueFamilies[familyIndex];

				if (queueFamily.queueCount == 0) {
					continue;
				}

				const vk::QueueFlags queueFlags = queueFamily.queueFlags;

				if (!indices.graphicsFamily &&
					static_cast<bool>(queueFlags & vk::QueueFlagBits::eGraphics)) {
					indices.graphicsFamily = familyIndex;
				}

				if (!indices.computeFamily &&
					static_cast<bool>(queueFlags & vk::QueueFlagBits::eCompute)) {
					indices.computeFamily = familyIndex;
				}

				if (static_cast<bool>(queueFlags & vk::QueueFlagBits::eCompute) &&
					!static_cast<bool>(queueFlags & vk::QueueFlagBits::eGraphics)) {
					indices.computeFamily = familyIndex;
				}

				if (!indices.transferFamily &&
					static_cast<bool>(queueFlags & vk::QueueFlagBits::eTransfer)) {
					indices.transferFamily = familyIndex;
				}

				if (static_cast<bool>(queueFlags & vk::QueueFlagBits::eTransfer) &&
					!static_cast<bool>(queueFlags & vk::QueueFlagBits::eGraphics) &&
					!static_cast<bool>(queueFlags & vk::QueueFlagBits::eCompute)) {
					indices.transferFamily = familyIndex;
				}

				if (requirements.requirePresent && surface) {
					const vk::Bool32 presentSupport = device.getSurfaceSupportKHR(familyIndex, surface);
					if (!indices.presentFamily && presentSupport) {
						indices.presentFamily = familyIndex;
					}
				}
			}

			return indices;
		}

		bool satisfiesRequirements(
			const VulkanQueueFamilyIndices& indices,
			const VulkanAdapterRequirements& requirements
		) {
			if (requirements.requireGraphics && !indices.hasGraphics()) {
				return false;
			}

			if (requirements.requirePresent && !indices.hasPresent()) {
				return false;
			}

			if (requirements.requireCompute && !indices.hasCompute()) {
				return false;
			}

			if (requirements.requireTransfer && !indices.hasTransfer()) {
				return false;
			}

			return true;
		}

		int scoreDevice(
			vk::PhysicalDevice device,
			const VulkanQueueFamilyIndices& indices
		) {
			const vk::PhysicalDeviceProperties properties = device.getProperties();
			int score = 0;

			if (properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu) {
				score += 100000;
			}
			else if (properties.deviceType == vk::PhysicalDeviceType::eIntegratedGpu) {
				score += 50000;
			}

			score += static_cast<int>(properties.limits.maxImageDimension2D);

			if (indices.graphicsFamily && indices.presentFamily &&
				indices.graphicsFamily.value() == indices.presentFamily.value()) {
				score += 1000;
			}

			if (indices.computeFamily && indices.graphicsFamily &&
				indices.computeFamily.value() != indices.graphicsFamily.value()) {
				score += 100;
			}

			if (indices.transferFamily && indices.graphicsFamily &&
				indices.transferFamily.value() != indices.graphicsFamily.value()) {
				score += 100;
			}

			return score;
		}
	}

	void VulkanAdapter::select(vk::Instance instance, const VulkanAdapterRequirements& requirements, vk::SurfaceKHR surface) {
		if (isvalid_) return;
		if (!instance) {
			EG_ERROR("VulkanAdapter::select: invalid instance");
			return;
		}
		if (requirements.requirePresent && !surface) {
			EG_ERROR("VulkanAdapter::select: present queue require valid surface");
			return;
		}
		auto devices = instance.enumeratePhysicalDevices();
		if (devices.empty()) {
			EG_ERROR("VulkanAdapter::select: no physical device found");
			return;
		}

		EG_INFO("VulkanAdapter::select: found {} physical device(s)", devices.size());

		vk::PhysicalDevice bestDevice = nullptr;
		VulkanQueueFamilyIndices bestQueueFamilies{};
		int bestScore = std::numeric_limits<int>::min();

		for (const vk::PhysicalDevice& device : devices) {
			const vk::PhysicalDeviceProperties deviceProperties = device.getProperties();
			const VulkanQueueFamilyIndices queueFamilies = findQueueFamilies(device, requirements, surface);

			EG_INFO(
				"VulkanAdapter::select: candidate '{}' ({})",
				physicalDeviceName(deviceProperties),
				deviceTypeName(deviceProperties.deviceType)
			);

			if (!satisfiesRequirements(queueFamilies, requirements)) {
				EG_WARN("VulkanAdapter::select: skipped '{}': missing required queue family", physicalDeviceName(deviceProperties));
				continue;
			}

			if (!hasDeviceExtensions(device, requirements)) {
				EG_WARN("VulkanAdapter::select: skipped '{}': missing required device extension", physicalDeviceName(deviceProperties));
				continue;
			}

			const int score = scoreDevice(device, queueFamilies);
			EG_INFO("VulkanAdapter::select: candidate '{}' score {}", physicalDeviceName(deviceProperties), score);

			if (score > bestScore) {
				bestDevice = device;
				bestQueueFamilies = queueFamilies;
				bestScore = score;
			}
		}

		if (!bestDevice) {
			EG_ERROR("VulkanAdapter::select: failed to find suitable physical device");
			return;
		}

		physicalDevice_ = bestDevice;
		properties_ = physicalDevice_.getProperties();
		features_ = physicalDevice_.getFeatures();
		queueFamilyIndices_ = bestQueueFamilies;
		isvalid_ = true;

		EG_INFO(
			"VulkanAdapter::select: selected '{}' ({})",
			physicalDeviceName(properties_),
			deviceTypeName(properties_.deviceType)
		);

		if (queueFamilyIndices_.graphicsFamily) {
			EG_INFO("VulkanAdapter::select: graphics queue family {}", queueFamilyIndices_.graphicsFamily.value());
		}

		if (queueFamilyIndices_.presentFamily) {
			EG_INFO("VulkanAdapter::select: present queue family {}", queueFamilyIndices_.presentFamily.value());
		}

		if (queueFamilyIndices_.computeFamily) {
			EG_INFO("VulkanAdapter::select: compute queue family {}", queueFamilyIndices_.computeFamily.value());
		}

		if (queueFamilyIndices_.transferFamily) {
			EG_INFO("VulkanAdapter::select: transfer queue family {}", queueFamilyIndices_.transferFamily.value());
		}

	}

	vk::PhysicalDevice VulkanAdapter::handle() const {
		return physicalDevice_;
	}
	const VulkanQueueFamilyIndices& VulkanAdapter::queueFamilyIndices() const {
		return queueFamilyIndices_;
	}
	const vk::PhysicalDeviceProperties& VulkanAdapter::properties() const {
		return properties_;
	}
	const vk::PhysicalDeviceFeatures& VulkanAdapter::features() const {
		return features_;
	}

	void VulkanAdapter::destroy() {
		physicalDevice_ = nullptr;
		properties_ = vk::PhysicalDeviceProperties();
		features_ = vk::PhysicalDeviceFeatures();
		queueFamilyIndices_ = {};
		isvalid_ = false;
	}

	bool VulkanAdapter::isValid() const {
		return isvalid_ && static_cast<bool>(physicalDevice_);
	}

	VulkanAdapter::operator bool() const {
		return isValid();
	}

}
