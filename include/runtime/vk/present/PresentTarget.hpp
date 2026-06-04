#pragma once

#include "runtime/vk/present/PresentTypes.hpp"
#include "runtime/vk/present/VulkanSurface.hpp"
#include <vulkan/vulkan.hpp>
namespace RT {
	class PresentTarget {
	private:

		VulkanSurface surface_{};
		PresentTargetDesc desc_{};
		
		bool resizePending_ = false;


	public:
		PresentTarget() = default;
		~PresentTarget();

		void createSurface(vk::Instance instance, const PresentTargetDesc& desc);
		void resize(uint32_t w, uint32_t h);
		void destroy();

		PresentTargetId Id() const;
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