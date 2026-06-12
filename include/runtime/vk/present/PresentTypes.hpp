#pragma once

#include <cstdint>
#include <runtime/platform/NativeWindowHandle.hpp>
#include "runtime/core/HandleTable.hpp"
#include "runtime/core/Handle.hpp"
#include <vulkan/vulkan.hpp>
namespace RT {
	struct PresentTargetTag {};
	using PresentTargetHandle = Handle<PresentTargetTag>;
	struct PresentTargetDesc {
		PL::NativeWindowHandle nativeHandle_{};
		uint32_t width_ = 0;
		uint32_t height_ = 0;
		bool vsync_ = true;

	};

}