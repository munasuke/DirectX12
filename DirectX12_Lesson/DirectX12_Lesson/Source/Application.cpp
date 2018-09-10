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
#include "Vertex.h"
#include "TextureResource.h"
#include "ShaderResourceView.h"
#include "BmpLoader.h"
#include "ShaderLoader.h"
#include "PipelineState.h"

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
	root			= std::make_shared<RootSignature>();
	fence			= std::make_shared<Fence>();
	vertex			= std::make_shared<Vertex>();
	tex				= std::make_shared<TextureResource>();
	srv				= std::make_shared<ShaderResourceView>();
	bmp				= std::make_shared<BmpLoader>();
	shader			= std::make_shared<ShaderLoader>();
	pipline			= std::make_shared<PipelineState>();
}

//������
void Application::Initialize() {
	//�E�B���h�E
	window->InitWindow();
	//�R�}���h
	command->InitCommand(device->GetDevice());
	//�X���b�v�`�F�C��
	swapChain->InitSwapChain(command->GetCommandQueue(), window->GetHandleWindow());
	//�f�X�N���v�^
	descriptor->InitDescriptor(device->GetDevice());
	//�����_�[�^�[�Q�b�g
	renderTarget->InitRenderTarget(swapChain->GetSwapChainDesc().BufferCount, 
		device->GetDevice(), swapChain->GetSwapChain(), descriptor->GetDescriptorHandle(), descriptor->GetDescriptorSize());
	//�T���v��
	sampler->InitSampler();
	//���[�g�V�O�l�`��
	root->InitRootSignature(sampler->GetSamplerDesc(), device->GetDevice(), D3D12_SHADER_VISIBILITY_ALL);
	//�t�F���X
	fence->InitFence(device->GetDevice());
	//���_�o�b�t�@
	vertex->Initialize(device->GetDevice());
	//�e�N�X�`�����\�[�X
	tex->Initialize(device->GetDevice());
	//�V�F�[�_���\�[�X�r���[
	srv->Initialize(device->GetDevice(), tex->GetTextureBuffer());
	//BMP
	bmp->Load("img/aoba.bmp");
	//�V�F�[�_
	shader->Load(root->GetError());
	//�p�C�v���C���X�e�[�g�I�u�W�F�N�g
	pipline->Initialize(device->GetDevice(), shader->GetVS(), shader->GetPS(), 
		vertex->GetInputDescNum(), vertex->GetInputDesc(), root->GetRootSignature());
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
		//���[�g�V�O�l�`���̃Z�b�g
		command->GetCommandList()->SetGraphicsRootSignature(root->GetRootSignature());

		//��ʂɐF��t����
		auto bbIndex = swapChain->GetSwapChain()->GetCurrentBackBufferIndex();
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(descriptor->GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(), 
			bbIndex, descriptor->GetDescriptorSize());
		command->GetCommandList()->OMSetRenderTargets(1, &rtvHandle, false, nullptr);
		const FLOAT color[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		command->GetCommandList()->ClearRenderTargetView(rtvHandle, color, 0, nullptr);


		command->Execute();

		swapChain->GetSwapChain()->Present(1, 0);
		command->GetCommandQueue()->Signal(fence->GetFence(), fence->GetFenceValue(true));
		while (fence->GetFence()->GetCompletedValue() != fence->GetFenceValue()){
			//�ҋ@
		}

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
