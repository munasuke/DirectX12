#include "Application.h"
#include <Windows.h>
#include <iostream>

int main(){

	std::cout << "‚Ô‚½" << std::endl;

	auto& app = Application::Instance();
	app.Initialize();
	app.Run();
	app.Terminate();

	return 0;
}