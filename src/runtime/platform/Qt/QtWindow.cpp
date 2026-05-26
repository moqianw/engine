#include "runtime/platform/Qt/QtWindow.hpp"


#include <QWindow>
#include <QString>
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
}