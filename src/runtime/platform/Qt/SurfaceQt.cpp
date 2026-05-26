#include "runtime/platform/Qt/SurfaceQt.hpp"

#include <QWindow>

#if defined(_WIN32)
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>
#include <vulkan/vulkan_win32.h>
#endif

namespace PL {
	vk::SurfaceKHR SurfaceQt::createVulkanSurface(vk::Instance instance, QWindow* window)
	{
		if (!instance || window == nullptr) {
			return nullptr;
		}

#if defined(_WIN32)
		return createVulkanSurface(instance, reinterpret_cast<void*>(window->winId()));
#else
		return nullptr;
#endif
	}

	vk::SurfaceKHR SurfaceQt::createVulkanSurface(vk::Instance instance, void* nativeWindowHandle)
	{
		if (!instance || nativeWindowHandle == nullptr) {
			return nullptr;
		}

#if defined(_WIN32)
		VkWin32SurfaceCreateInfoKHR surfInfo{};
		surfInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		surfInfo.hwnd = static_cast<HWND>(nativeWindowHandle);
		surfInfo.hinstance = GetModuleHandle(nullptr);

		VkSurfaceKHR cSurface = VK_NULL_HANDLE;
		const VkResult result =
			vkCreateWin32SurfaceKHR(static_cast<VkInstance>(instance), &surfInfo, nullptr, &cSurface);
		if (result != VK_SUCCESS || cSurface == VK_NULL_HANDLE) {
			return nullptr;
		}

		return vk::SurfaceKHR(cSurface);
#else
		return nullptr;
#endif
	}
}
