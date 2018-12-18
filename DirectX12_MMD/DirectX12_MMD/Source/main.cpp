#include "Application.h"
#include <Windows.h>
#include <iostream>

int main() {
	auto& app = Application::Instance();
	app.Initialize();
	app.Run();
	app.Terminate();

	return 0;
}