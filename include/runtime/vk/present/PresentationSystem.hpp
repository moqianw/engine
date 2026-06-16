#pragma once
#include "runtime/vk/present/PresentTarget.hpp"
#include "runtime/core/HandleTable.hpp"
#include <vulkan/vulkan.hpp>
namespace RT {
	using PresentTargetTable = HandleTable<PresentTargetHandle, PresentTarget>;
	class VulkanCore;
	class PresentationSystem {
	private:
		PresentTargetTable table_{};
		const VulkanCore* vkCore_ = nullptr;
		bool isvalid_ = false;
	public:
		void create(const VulkanCore* core);
		void destroy();
		PresentTargetHandle createTarget(const PresentTargetDesc& desc);
		bool destroyTarget(const PresentTargetHandle handle);
		bool resizeTarget(const PresentTargetHandle handle,uint32_t width, uint32_t height);
		PresentTarget* getTarget(const PresentTargetHandle handle);
		const PresentTarget* getTarget(const PresentTargetHandle handle) const;
		bool hasTarget(const PresentTargetHandle handle) const;

		bool isValid() const;
		explicit operator bool() const;
	};
}