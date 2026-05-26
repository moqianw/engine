#include <QApplication>

#include "runtime/platform/Qt/QtWindow.hpp"
int main(int argc, char** args) {
	QApplication* app = new QApplication(argc, args);
	PL::WindowDesc desc{};
	PL::QtWindow window(desc);
	window.show();
	return app->exec();
}