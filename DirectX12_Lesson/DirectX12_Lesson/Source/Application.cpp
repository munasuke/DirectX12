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
#include "ViewPort.h"
#include "Matrix.h"

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
	viewPort		= std::make_shared<ViewPort>();
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
	bmp->Load("Image/aoba.bmp");
	//�V�F�[�_
	shader->Load(root->GetError());
	//�p�C�v���C���X�e�[�g�I�u�W�F�N�g
	pipline->Initialize(device->GetDevice(), shader->GetVS(), shader->GetPS(), 
		vertex->GetInputDescNum(), vertex->GetInputDesc(), root->GetRootSignature());
	//�r���[�|�[�g
	viewPort->Initialize();
}

//���C�����[�v
void Application::Run() {
	window->Show();
	while(msg.message != WM_QUIT){
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)){
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		//���߂̃N���A
		command->GetCommandAllocator()->Reset();
		command->GetCommandList()->Reset(command->GetCommandAllocator(), pipline->GetPiplineState());

		//���[�g�V�O�l�`���̃Z�b�g
		command->GetCommandList()->SetGraphicsRootSignature(root->GetRootSignature());
		//�p�C�v���C���̃Z�b�g
		command->GetCommandList()->SetPipelineState(pipline->GetPiplineState());
		//�r���[�|�[�g�̃Z�b�g
		command->GetCommandList()->RSSetViewports(1, &viewPort->GetViewPort());
		//�V�U�[�̃Z�b�g
		D3D12_RECT scissorRect = { 0, 0, WIN_WIDTH, WIN_HEIGHT };
		command->GetCommandList()->RSSetScissorRects(1, &window->GetScissorRect());

		//SRV�p�̃f�X�N���v�^���Z�b�g
		command->GetCommandList()->SetDescriptorHeaps(1, srv->GetTextureHeap2());
		command->GetCommandList()->SetGraphicsRootDescriptorTable(0, srv->GetTextureHeap()->GetGPUDescriptorHandleForHeapStart());

		//�o���A�𒣂�
		command->GetCommandList()->ResourceBarrier(
			0,
			&CD3DX12_RESOURCE_BARRIER::Transition(
				renderTarget->GetRenderTarget()[swapChain->GetSwapChain()->GetCurrentBackBufferIndex()], 
				D3D12_RESOURCE_STATE_PRESENT, 
				D3D12_RESOURCE_STATE_RENDER_TARGET)
		);

		//���_�o�b�t�@�̃Z�b�g
		command->GetCommandList()->IASetVertexBuffers(0, 1, &vertex->GetVBV());

		//��ʂɐF��t����
		//�o�b�N�o�b�t�@�̃C���f�b�N�X���擾
		auto bbIndex = swapChain->GetSwapChain()->GetCurrentBackBufferIndex();

		//�����_�[�^�[�Q�b�g�̎w��
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(descriptor->GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(), 
			bbIndex, descriptor->GetDescriptorSize());
		command->GetCommandList()->OMSetRenderTargets(1, &rtvHandle, false, nullptr);

		//�N���A�J���[�̐ݒ�
		const FLOAT color[] = { 0.0f, 0.0f, 0.0f, 1.0f };

		//�����_�[�^�[�Q�b�g�̃N���A
		command->GetCommandList()->ClearRenderTargetView(rtvHandle, color, 0, nullptr);

		//�O�p�|���S���`��ɂ���
		command->GetCommandList()->IASetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//�e�N�X�`���o�b�t�@�ւ̏�������
		tex->WriteToTextureBuffer(bmp->GetData());

		//���_�`��
		command->GetCommandList()->DrawInstanced(6, 1, 0, 0);

		//�o���A�𒣂�
		command->GetCommandList()->ResourceBarrier(
			0,
			&CD3DX12_RESOURCE_BARRIER::Transition(
				tex->GetTextureBuffer(),
				D3D12_RESOURCE_STATE_COPY_DEST,
				D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE)
		);

		//�R�}���h���X�g�����
		command->GetCommandList()->Close();

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
