#include "Application.h"
#include "Window.h"
#include "Result.h"
#include "Device.h"
#include "Command.h"
#include "SwapChain.h"
#include "Descriptor.h"
#include "RenderTarget.h"
#include "Sampler.h"
#include "DescriptorRange.h"
#include "Parameter.h"
#include "RootSignature.h"
#include "Fence.h"

namespace{
	MSG msg = {};
}

Application::Application() {
	window			= std::make_shared<Window>();
	device			= std::make_shared<Device>();
	command			= std::make_shared<Command>();
	swapChain		= std::make_shared<SwapChain>();
	descriptor		= std::make_shared<Descriptor>();
	renderTarget	= std::make_shared<RenderTarget>();
	sampler			= std::make_shared<Sampler>();
	dRange			= std::make_shared<DescriptorRange>();
	parameter		= std::make_shared<Parameter>();
	root			= std::make_shared<RootSignature>();
	fence			= std::make_shared<Fence>();
}

//初期化
void Application::Initialize() {
	window->InitWindow();
	command->InitCommand(device->GetDevice());
	swapChain->InitSwapChain(command->GetCommandQueue(), window->GetHandleWindow());
	descriptor->InitDescriptor(device->GetDevice());
	renderTarget->InitRenderTarget(swapChain->GetSwapChainDesc().BufferCount, device->GetDevice(), swapChain->GetSwapChain(), descriptor->GetDescriptorHandle(), descriptor->GetDescriptorSize());
	sampler->InitSampler();
	dRange->InitDescriptorRange();
	parameter->InitParameter(D3D12_SHADER_VISIBILITY_ALL, dRange->GetDescriptorRange(0));
	root->InitRootSignature(parameter->GetParamatorSize(), parameter->GetParameter(), sampler->GetSamplerDesc(), device->GetDevice());
	fence->InitFence(device->GetDevice());
}

//メインループ
void Application::Run() {
	window->Show();
	while(msg.message != WM_QUIT){
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)){
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		command->GetCommandAllocator()->Reset();
		//ルートシグネチャのセット
		command->GetCommandList()->SetGraphicsRootSignature(root->GetRootSignature());


		command->Execute();

		swapChain->GetSwapChain()->Present(1, 0);
		command->GetCommandQueue()->Signal(fence->GetFence(), fence->GetFenceValue(true));
		while (fence->GetFence()->GetCompletedValue() != fence->GetFenceValue()){
			//待機
		}

		//ゲームの処理
		//ゲームの進行
		//描画処理
	}
}

//終了
void Application::Terminate() {
}

Application::~Application() {
}
