#pragma once
#include <vulkan/vulkan.hpp>
#include "runtime/platform/NativeWindowHandle.hpp"
namespace RT {
	class VulkanSurface {
	private:
		vk::SurfaceKHR surface_ = nullptr;
		vk::Instance instance_ = nullptr;
	public:
		VulkanSurface() = default;
		~VulkanSurface() = default;

		void create(vk::Instance instance, const PL::NativeWindowHandle& handle);
		void destroy();
		vk::SurfaceKHR handle() const;

		bool isValid() const;

		explicit  operator bool() const;
	};
}