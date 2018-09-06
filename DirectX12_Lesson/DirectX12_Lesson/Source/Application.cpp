#include "Application.h"
#include "Window.h"
#include "Result.h"
#include "Device.h"
#include "Command.h"
#include "SwapChain.h"
#include "Descriptor.h"

namespace{
	MSG msg = {};
}

Application::Application() {
	window = std::make_shared<Window>();
	device = std::make_shared<Device>();
	command = std::make_shared<Command>();
	swapChain = std::make_shared<SwapChain>();
	descriptor = std::make_shared<Descriptor>();
}

//������
void Application::Initialize() {
	window->InitWindow();
	command->InitCommand(device->GetDevice());
	swapChain->InitSwapChain(command->GetCommandQueue(), window->GetHandleWindow());
	descriptor->InitDescriptor(device->GetDevice());
}

//���C�����[�v
void Application::Run() {
	window->Show();
	while(msg.message != WM_QUIT){
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)){
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		command->GetCommandAllocator()->Reset();

		//�Q�[���̏���
		//�Q�[���̐i�s
		//�`�揈��
	}
}

//�I��
void Application::Terminate() {
}

Application::~Application() {
}
