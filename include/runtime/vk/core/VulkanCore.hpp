#pragma once
#include "runtime/vk/core/VulkanAdapter.hpp"
#include "runtime/vk/core/VulkanDevice.hpp"
#include "runtime/vk/core/VulkanInstance.hpp"

namespace RT {
	class VulkanCore {
	private:
		VulkanInstance instance_{};
		VulkanAdapter adapter_{};
		VulkanDevice device_{};
		bool isvalid_ = false;
	public:
		VulkanCore() = default;
		~VulkanCore() = default;
		void create();
		void createDevice(const VulkanDeviceRequirements& deviceRequirements, vk::SurfaceKHR surface = nullptr);
		void destroy();


		bool isValid() const;
		bool hasAdapter() const;
		bool hasDevice() const;
		explicit operator bool() const;

		const VulkanInstance& vulkanInstance() const;
		VulkanInstance& vulkanInstance();

		const VulkanAdapter& vulkanAdapter() const;
		VulkanAdapter& vulkanAdapter();

		const VulkanDevice& vulkanDevice() const;
		VulkanDevice& vulkanDevice();
	};
}
