#pragma once

#include <vulkan/vulkan.hpp>

namespace RT {
	class VulkanInstance {
	private:
		vk::Instance instance_ = nullptr;
	public:
		VulkanInstance() = default;
		~VulkanInstance() = default;

		void create();
		void destroy();

		vk::Instance handle() const;
		vk::Instance handle();
		bool isValid() const;
		explicit operator bool() const;
	};
}