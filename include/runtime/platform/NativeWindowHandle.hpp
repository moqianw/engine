#pragma once
#include <variant>
namespace PL {
	enum class NativeWindowBackendFlags {
		UnKnown,
		Win32
	};
	
	struct NativeWindowHandle {
		NativeWindowBackendFlags backend_ = NativeWindowBackendFlags::UnKnown;
		union {
			struct{
				void* windowHandle_ = nullptr;
				void* moduleHandle_ = nullptr;
			}win32;
		};
		bool isValid() const {
			return backend_ != NativeWindowBackendFlags::UnKnown && win32.windowHandle_ && win32.moduleHandle_;
		}
	};
}