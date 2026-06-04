#pragma once

#include <cstdint>
#include <runtime/platform/NativeWindowHandle.hpp>
namespace RT {
	using PresentTargetId = uint32_t;


	struct PresentTargetDesc {
		PresentTargetId id_ = 0;
		PL::NativeWindowHandle nativeHandle_{};
		uint32_t width_ = 0;
		uint32_t height_ = 0;
		bool vsync_ = true;

	};
}