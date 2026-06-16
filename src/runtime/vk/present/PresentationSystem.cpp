#include "runtime/vk/present/PresentationSystem.hpp"
#include "runtime/vk/core/VulkanCore.hpp"
#include "core/Log.hpp"
namespace RT {
	void PresentationSystem::create(const VulkanCore* core) {
		if (isvalid_) return;
		if (!core || !core->isValid()) {
			EG_ERROR("PresentationSystem::create: vulkan instance invalid");
			return;
		}
		vkCore_ = core;
		isvalid_ = true;
		EG_INFO("PresentationSystem created");
	}
	void PresentationSystem::destroy() {
		if (!isvalid_) {
			return;
		}

		const std::vector<PresentTargetHandle> handles = table_.aliveHandles();

		for (const PresentTargetHandle& handle : handles) {
			destroyTarget(handle);
		}

		table_.clear();
		vkCore_ = nullptr;
		isvalid_ = false;

		EG_INFO("PresentationSystem destroyed");
	}
	PresentTargetHandle PresentationSystem::createTarget(const PresentTargetDesc& desc) {
		if (!isValid()) {
			EG_ERROR("PresentationSystem::createTarget: system is not ready");
			return PresentTargetHandle::invalid();
		}
		auto handle = table_.allocate();
		auto* target = table_.getReserved(handle);
		if (!target) {
			EG_ERROR("PresentationSystem::createTarget: target invalid");
			table_.cancel(handle);
			return PresentTargetHandle::invalid();
		}
		///
		if (!target->create(nullptr ,vkCore_, desc)) {
			table_.cancel(handle);
			target->destroy();
			EG_ERROR("PresentationSystem::createTarget: create surface false");
			return PresentTargetHandle::invalid();
		}
		if (!table_.commit(handle)) {
			table_.cancel(handle);
			target->destroy();
			EG_ERROR("PresentationSystem::createTarget: table commit false");
			return PresentTargetHandle::invalid();
		}
		return handle;
	}
	bool PresentationSystem::destroyTarget(const PresentTargetHandle handle) {
		auto* target = table_.get(handle);
		if (!target) return false;
		
		table_.releaseForce(handle);
		target->destroy();
		return true;
	}
	bool PresentationSystem::resizeTarget(const PresentTargetHandle handle, uint32_t width, uint32_t height) {
		auto* target = table_.get(handle);
		if (!target) return false;
		target->resize(width, height);
		return true;
	}
	PresentTarget* PresentationSystem::getTarget(const PresentTargetHandle handle) {
		return table_.get(handle);
	}
	const PresentTarget* PresentationSystem::getTarget(const PresentTargetHandle handle) const {
		return table_.get(handle);
	}
	bool PresentationSystem::hasTarget(const PresentTargetHandle handle) const {
		return table_.contains(handle);
	}

	bool PresentationSystem::isValid() const {
		return isvalid_ && vkCore_ && vkCore_->isValid();
	}
	PresentationSystem::operator bool() const {
		return isValid();
	}
}