#pragma once

#include "runtime/vk/present/PresentTypes.hpp"
#include "runtime/vk/present/VulkanSurface.hpp"
#include "runtime/vk/present/VulkanSwapchain.hpp"
#include <vulkan/vulkan.hpp>
namespace RT {
	class PresentTarget {
	private:

		VulkanSurface surface_{};
		VulkanSwapchain swapchain_{};
		PresentTargetDesc desc_{};
		
		bool resizePending_ = false;


	public:
		PresentTarget() = default;
		~PresentTarget() = default;

		bool createSurface(vk::Instance instance, const PresentTargetDesc& desc);
		bool resize(uint32_t w, uint32_t h);
		void destroy();

		VulkanSurface& surface();
		const VulkanSurface& surface() const;

		uint32_t width() const;
		uint32_t height() const;
		bool vsync() const;

		bool resizePending() const;
		bool isSurfaceValid() const;
		bool isValid() const;
		
	};
}