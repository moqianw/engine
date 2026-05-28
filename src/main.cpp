#include <QApplication>

#include "runtime/platform/Qt/QtWindow.hpp"
#include "runtime/vk/VulkanBackend.hpp"
int main(int argc, char** args) {
	QApplication* app = new QApplication(argc, args);
	RT::VulkanBackend vkbackend{};
	vkbackend.create();

	PL::WindowDesc desc{};
	PL::QtWindow* window = new PL::QtWindow(desc);
	window->show();

	vkbackend.pushtestsurface(window);
	app->exec();
	delete window;
	vkbackend.destroy();
	return 0;
}