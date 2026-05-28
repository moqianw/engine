#include "runtime/platform/Qt/QtWindow.hpp"


#include <QWindow>
#include <QString>
#if defined(_WIN32)
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif
namespace PL {
	QtWindow::QtWindow(const WindowDesc& desc) {
		window_ = new QWindow();
		window_->setWidth(desc.width_);
		window_->setHeight(desc.height_);
		window_->setTitle(QString::fromStdString(desc.title_));
		window_->setFlags(Qt::Window);
		window_->setSurfaceType(QWindow::VulkanSurface);
	}

	QtWindow::~QtWindow() {
		delete window_;
	}
	void QtWindow::show() {
		window_->show();
	}
	uint32_t QtWindow::height() const { return window_->height(); }
	uint32_t QtWindow::width() const { return window_->width(); }
	QWindow* QtWindow::window() const { return window_; }
	NativeWindowHandle QtWindow::nativeHandle() const
	{
		NativeWindowHandle handle{};
#if defined(_WIN32)
		handle.backend_ = NativeWindowBackendFlags::Win32;
		handle.win32.windowHandle_ = reinterpret_cast<void*>(window_->winId());
		handle.win32.moduleHandle_ = reinterpret_cast<void*>(GetModuleHandle(nullptr));
#endif
		return handle;
	}
}