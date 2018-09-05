#include "Application.h"
#include "Window.h"
#include "Device.h"
#include "Command.h"

namespace{
	MSG msg = {};
}

Application::Application() {
	window = std::make_shared<Window>();
	device = std::make_shared<Device>();
	command = std::make_shared<Command>(device->dev, device->result);
}

void Application::Initialize() {
	window->InitWindow();
}

void Application::Run() {
	window->Show();
	while(msg.message != WM_QUIT){
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)){
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}

void Application::Terminate() {
}

Application::~Application() {
}
