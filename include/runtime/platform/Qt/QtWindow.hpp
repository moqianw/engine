#pragma once
#include <cstdint>
#include <string>
#include "runtime/platform/NativeWindowHandle.hpp"
class QWindow;
namespace PL {
	struct WindowDesc {
		uint32_t width_ = 1080;
		uint32_t height_ = 960;
		std::string title_ = "viewport";
	};

	class QtWindow {
	private:
		QWindow* window_ = nullptr;
	public:
		explicit QtWindow(const WindowDesc& desc);
		~QtWindow();
		
		void show();

		uint32_t width() const;
		uint32_t height() const;
		QWindow* window() const;
		NativeWindowHandle nativeHandle() const;
	};
}