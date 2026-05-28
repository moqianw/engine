#include <QApplication>
#include "core/Log.hpp"
#include "runtime/platform/Qt/QtWindow.hpp"
#include "runtime/vk/VulkanBackend.hpp"
int main(int argc, char** args) {
	QApplication* app = new QApplication(argc, args);
	EG::Log::init();
	EG_INFO("Application starting...");
	RT::VulkanBackend vkbackend{};
	vkbackend.create();

	PL::WindowDesc desc{};
	PL::QtWindow* window = new PL::QtWindow(desc);
	window->show();

	app->exec();
	vkbackend.destroy();
	return 0;
}