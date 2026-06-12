#include "runtime/vk/present/PresentationSystem.hpp"
#include "core/Log.hpp"
namespace RT {
	void PresentationSystem::create(vk::Instance instance) {
		if (isvalid_) return;
		if (!instance) {
			EG_ERROR("PresentationSystem::create: vulkan instance invalid");
			return;
		}
		instance_ = instance;
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
		instance_ = nullptr;
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
		if (!target->createSurface(instance_, desc)) {
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
		return isvalid_ && static_cast<bool>(instance_);
	}
	PresentationSystem::operator bool() const {
		return isValid();
	}
}