#include "runtime/vk/present/VulkanSurface.hpp"
#if defined(_WIN32)
#include <vulkan/vulkan_win32.h>
#endif

#include "core/Log.hpp"

namespace RT {

	void VulkanSurface::create(vk::Instance instance, const PL::NativeWindowHandle& handle) {
		if (isValid()) return;
		if (!instance || !handle.isValid()) return;
		instance_ = instance;

#if defined(_WIN32)
		VkWin32SurfaceCreateInfoKHR surfInfo{};
		surfInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		surfInfo.hwnd = static_cast<HWND>(handle.win32.windowHandle_);
		surfInfo.hinstance = static_cast<HINSTANCE>(handle.win32.moduleHandle_);
		

		VkSurfaceKHR cSurface = VK_NULL_HANDLE;
		const VkResult result =
			vkCreateWin32SurfaceKHR(static_cast<VkInstance>(instance), &surfInfo, nullptr, &cSurface);
		if (result != VK_SUCCESS || cSurface == VK_NULL_HANDLE) {
			EG_ERROR("VulkanSurface::create: false");
			return;
		}

		surface_ =  vk::SurfaceKHR(cSurface);
		EG_TRACE("VulkanSurface created");
#else
		return;
#endif
	}
	void VulkanSurface::destroy() {
		if (!isValid()) return;
		instance_.destroySurfaceKHR(surface_);
		surface_ = nullptr;
		instance_ = nullptr;

	}
	vk::SurfaceKHR VulkanSurface::handle() const {
		return surface_;
	}

	bool VulkanSurface::isValid() const {
		return static_cast<bool>(instance_) && static_cast<bool>(surface_);
	}

	VulkanSurface::operator bool() const {
		return isValid();
	}
}