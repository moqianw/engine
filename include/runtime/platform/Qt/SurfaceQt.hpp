#pragma once
#include <vulkan/vulkan.hpp>

class QWindow;
namespace PL {
	class SurfaceQt {
	public:
		SurfaceQt() = default;
		~SurfaceQt() = default;
		static vk::SurfaceKHR createVulkanSurface(vk::Instance instance, QWindow* window);
		static vk::SurfaceKHR createVulkanSurface(vk::Instance instance, void* nativeWindowHandle);

	};
}
