#include "Application.h"
#include "Window.h"

namespace{
	MSG msg = {};
}

Application::Application() {
}


void Application::Initialize() {
	window = std::make_shared<Window>();
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
