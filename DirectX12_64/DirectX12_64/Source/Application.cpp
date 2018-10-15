#define _ENABLE_EXTENDED_ALIGNED_STORAGE
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
#include "ImageLoader.h"
#include "ShaderLoader.h"
#include "PipelineState.h"
#include "ViewPort.h"
#include "ConstantBuffer.h"
#include "PMDLoader.h"
#include "Index.h"
#include "DepthStencilBuffer.h"
//#include <DirectXTex/DirectXTex.h>
//#pragma comment(lib, "DirectXTex.lib")

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
	imageL			= std::make_shared<ImageLoader>();
	shader			= std::make_shared<ShaderLoader>();
	pipline			= std::make_shared<PipelineState>();
	viewPort		= std::make_shared<ViewPort>();
	cb				= std::make_shared<ConstantBuffer>();
	pmd				= std::make_shared<PMDLoader>(bmp, imageL);
	index			= std::make_shared<Index>();
	depth			= std::make_shared<DepthStencilBuffer>();
}

//������
void Application::Initialize() {
	//�E�B���h�E�̃��f�B�A���g�����߂ɕK�v
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

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
	root->InitRootSignature(sampler->GetSamplerDesc(), device->GetDevice(), D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_ALL);

	//�t�F���X
	fence->InitFence(device->GetDevice());

	//PMD
	//pmd->Load("PMD/reimu/reimu_F01.pmd");
	pmd->Load("PMD/miku/初音ミク.pmd");
	//pmd->Load("PMD/neru/亞北ネル.pmd");
	//pmd->Load("PMD/hibari/�_��Ver1.10.pmd");

	//BMP
	//bmp->Load("PMD/miku/eye2.bmp");

	//�摜���[�h
	//imageL->Load(L"Image/real_eye.png");

	//���_�o�b�t�@
	vertex->Initialize(device->GetDevice(), pmd->GetPMDVertex());

	//�C���f�b�N�X
	index->Initialize(device->GetDevice(), pmd->GetIndices());

	//�e�N�X�`�����\�[�X
	tex->Initialize(device->GetDevice(), imageL->GetMetaData());

	//�V�F�[�_���\�[�X�r���[
	srv->Initialize(device->GetDevice(), tex->GetTextureBuffer(), pmd->GetMaterial().size());

	//�R���X�^���g�o�b�t�@
	cb->Initialize(device->GetDevice(), srv->GetTextureHeap());

	//�[�x�o�b�t�@
	depth->Initialize(device->GetDevice(), srv->GetDescriptorHeapDesc());

	//PMD������
	pmd->Initialize(device->GetDevice());

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
		command->GetCommandList()->RSSetScissorRects(1, &window->GetScissorRect());

		//�o���A�𒣂�
		command->GetCommandList()->ResourceBarrier(
			1,
			&CD3DX12_RESOURCE_BARRIER::Transition(
				renderTarget->GetRenderTarget()[swapChain->GetSwapChain()->GetCurrentBackBufferIndex()],
				D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_PRESENT,
				D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_RENDER_TARGET)
		);

		//���ʂɐF���t����
		//�o�b�N�o�b�t�@�̃C���f�b�N�X���擾
		auto bbIndex = swapChain->GetSwapChain()->GetCurrentBackBufferIndex();

		//�����_�[�^�[�Q�b�g�̎w��
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(descriptor->GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(),
			bbIndex, descriptor->GetDescriptorSize());
		command->GetCommandList()->OMSetRenderTargets(1, &rtvHandle, false, &depth->GetHeap()->GetCPUDescriptorHandleForHeapStart());

		//�N���A�J���[�̐ݒ�
		const FLOAT color[] = { 0.0f, 0.0f, 0.0f, 1.0f };

		//�����_�[�^�[�Q�b�g�̃N���A
		command->GetCommandList()->ClearRenderTargetView(rtvHandle, color, 0, nullptr);

		//�O�p�|���S���`���ɂ���
		command->GetCommandList()->IASetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//���_�o�b�t�@�̃Z�b�g
		command->GetCommandList()->IASetVertexBuffers(0, 1, &vertex->GetVBV());

		//�C���f�b�N�X�o�b�t�@�̃Z�b�g
		command->GetCommandList()->IASetIndexBuffer(&index->GetIndexBufferView());

		//�[�x�o�b�t�@�̃Z�b�g
		depth->SetDescriptor(command->GetCommandList());

		//SRV,�R���X�^���g�o�b�t�@�̃f�X�N���v�^���Z�b�g
		cb->UpDataWVP();
		for (UINT i = 0; i < 2; i++){
			cb->SetDescriptor(command->GetCommandList(), i, srv->GetTextureHeap(), device->GetDevice());
		}

		//�e�N�X�`���o�b�t�@�ւ̏�������
		tex->WriteToTextureBuffer(imageL->GetMetaData(), imageL->GetScratchImage(), pmd->GetTexFlag());

		//PMD�`��
		pmd->Draw(command->GetCommandList(), device->GetDevice(), srv->GetTextureHeap());

		//�o���A�𒣂�
		command->GetCommandList()->ResourceBarrier(
			1,
			&CD3DX12_RESOURCE_BARRIER::Transition(
				renderTarget->GetRenderTarget()[swapChain->GetSwapChain()->GetCurrentBackBufferIndex()],
				D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_RENDER_TARGET,
				D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_PRESENT)
		);

		//�R�}���h���X�g������
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
	CoUninitialize();
}

Application::~Application() {
}
