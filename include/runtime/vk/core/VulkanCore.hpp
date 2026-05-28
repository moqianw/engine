#pragma once
#include "runtime/vk/core/VulkanInstance.hpp"
namespace RT {
	class VulkanCore {
	private:
		VulkanInstance instance_{};
		bool isvalid_ = false;
	public:
		VulkanCore() = default;
		void create();
		void destroy();
		~VulkanCore();
		bool isValid() const;
		explicit operator bool() const;

		const VulkanInstance& vulkanInstance() const;
		VulkanInstance& vulkanInstance();
	};
}