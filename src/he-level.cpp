#include "app.h"

int main() {
	ulvl::Application* app = new ulvl::Application;
	app->init();
	app->loop();
	delete app;
	return 0;
}
