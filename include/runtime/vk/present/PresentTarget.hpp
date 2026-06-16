#pragma once

#include "runtime/vk/present/PresentTypes.hpp"
#include "runtime/vk/present/VulkanSwapchain.hpp"
#include <vulkan/vulkan.hpp>
namespace RT {
	class VulkanSurface;
	class VulkanCore;
	class PresentTarget {
	private:

		const VulkanSurface* surface_ = nullptr;
		const VulkanCore* vkCore_ = nullptr;
		VulkanSwapchain swapchain_{};
		PresentTargetDesc desc_{};
		
		bool resizePending_ = false;
		bool isvalid_ = false;

	public:
		PresentTarget() = default;
		~PresentTarget() = default;

		bool create(const VulkanSurface* surface, const VulkanCore* core, const PresentTargetDesc& desc);
		bool resize(uint32_t w, uint32_t h);
		bool createSwapchain();
		bool recreateSwapchain();
		void destroy();

		const VulkanSurface& surface() const;
		const VulkanSwapchain& swapchain() const;
		VulkanSwapchain& swapchain();
		uint32_t width() const;
		uint32_t height() const;
		bool vsync() const;
		bool resizePending() const;
		bool isValid() const;
		
	};
}