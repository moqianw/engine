#include "runtime/vk/present/PresentTarget.hpp"
#include "core/Log.hpp"
namespace RT {

	bool PresentTarget::createSurface(vk::Instance instance, const PresentTargetDesc& desc) {
		if (isSurfaceValid()) return true;
		desc_ = desc;
		surface_.create(instance, desc.nativeHandle_);
		if (!isSurfaceValid()) {
			EG_ERROR("PresentTarget::createSurface: failed create surface");
			return false;
		}
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
		if (surface_.isValid()) {
			surface_.destroy();
		}
		desc_ = {};
		resizePending_ = false;
	}


	VulkanSurface& PresentTarget::surface() {
		return surface_;
	}
	const VulkanSurface& PresentTarget::surface() const {
		return surface_;
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
	bool PresentTarget::isSurfaceValid() const {
		return surface_.isValid();
	}
	bool PresentTarget::isValid() const {
		return !resizePending_ && surface_.isValid();
	}
}