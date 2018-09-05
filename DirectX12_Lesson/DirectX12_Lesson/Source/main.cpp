#include "Application.h"
#include <Windows.h>
#include <iostream>

using namespace std;

int main(){

	cout << "‚Ô‚½" << endl;

	auto& app = Application::Instance();
	app.Initialize();
	app.Run();
	app.Terminate();

	return 0;
}