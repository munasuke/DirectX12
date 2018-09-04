#include "Application.h"
#include <Windows.h>
#include <iostream>

using namespace std;

int main(){

	cout << "‚Ô‚½" << endl;

	Application::Instance().Initialize();
	Application::Instance().Run();
	Application::Instance().Terminate();

	return 0;
}