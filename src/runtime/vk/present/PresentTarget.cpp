#include "runtime/vk/present/PresentTarget.hpp"
#include "runtime/vk/core/VulkanCore.hpp"
#include "runtime/vk/present/VulkanSurface.hpp"
#include "core/Log.hpp"
namespace RT {

	bool PresentTarget::create(const VulkanSurface* surface, const VulkanCore* core, const PresentTargetDesc& desc) {
		if (isvalid_) return true;
		desc_ = desc;
		if (!surface || !surface->isValid() || !core || core->isValid()) {
			EG_ERROR("PresentTarget::create: input invalid");
			return false;
		}
		vkCore_ = core;
		surface_ = surface;
		if (!surface_->isValid()) {
			EG_ERROR("PresentTarget::create: surface invalid");
			return false;
		}
		createSwapchain();
		return true;
	}
	bool PresentTarget::createSwapchain() {
		if (swapchain_.isValid()) {
			EG_ERROR("PresentTarget::createSwapchain: swapchain valid");
			return false;
		}
		swapchain_.create(vkCore_->vulkanDevice().handle(),
			surface_->handle(),
			vkCore_->vulkanAdapter().handle(),
			vkCore_->vulkanDevice().graphicsQueue().familyIndex(),
			vkCore_->vulkanDevice().presentQueue().familyIndex());
		if (!swapchain_.isValid()) {
			EG_ERROR("PresentTarget::createSwapchain: swapchain created false");
			return false;
		}
		EG_INFO("PresentTarget::createSwapchain: swapchain created");
		return true;
	}
	bool PresentTarget::recreateSwapchain() {
		if (!swapchain_.isValid()) {
			EG_ERROR("PresentTarget::recreateSwapchain: swapchain invalid");
			return false;
		}
		swapchain_.recreate(vkCore_->vulkanDevice().handle(),
			surface_->handle(),
			vkCore_->vulkanAdapter().handle(),
			vkCore_->vulkanDevice().graphicsQueue().familyIndex(),
			vkCore_->vulkanDevice().presentQueue().familyIndex());
		if (!swapchain_.isValid()) {
			EG_ERROR("PresentTarget::recreateSwapchain: swapchain recreated false");
			return false;
		}
		EG_INFO("PresentTarget::recreateSwapchain: swapchain recreated");
		return true;
	}
	bool PresentTarget::resize(uint32_t w, uint32_t h) {
		if (!w || !h) return false;
		if (w == desc_.width_ && h == desc_.height_) return false;
		desc_.width_ = w;
		desc_.height_ = h;
		resizePending_ = true;
		return true;
	}
	void PresentTarget::destroy() {
		if (swapchain_.isValid()) {
			swapchain_.destroy(vkCore_->vulkanDevice().handle());
		}
		surface_ = nullptr;
		desc_ = {};
		resizePending_ = false;
	}


	const VulkanSurface& PresentTarget::surface() const {
		return *surface_;
	}

	uint32_t PresentTarget::width() const {
		return desc_.width_;
	}
	uint32_t PresentTarget::height() const {
		return desc_.height_;
	}
	bool PresentTarget::vsync() const {
		return desc_.vsync_;
	}

	bool PresentTarget::resizePending() const {
		return resizePending_;
	}
	bool PresentTarget::isValid() const {
		return !resizePending_ && surface_->isValid();
	}
}