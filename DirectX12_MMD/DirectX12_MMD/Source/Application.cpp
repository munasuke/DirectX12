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
#include "BmpLoader.h"
#include "ImageLoader.h"
#include "ShaderLoader.h"
#include "PipelineState.h"
#include "ViewPort.h"
#include "PMDLoader.h"
#include "Index.h"
#include "DepthStencilBuffer.h"
#include "Model.h"
#include "Camera.h"
#include "VMDMotion.h"

namespace {
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
	bmp				= std::make_shared<BmpLoader>();
	imageL			= std::make_shared<ImageLoader>(device->GetDevice());
	shader			= std::make_shared<ShaderLoader>();
	pipline			= std::make_shared<PipelineState>();
	viewPort		= std::make_shared<ViewPort>();
	pmd				= std::make_shared<PMDLoader>(bmp, imageL);
	vmd				= std::make_shared<VMDMotion>();
	index			= std::make_shared<Index>();
	depth			= std::make_shared<DepthStencilBuffer>();
	model			= std::make_shared<Model>(pmd, imageL, vmd);
	camera			= std::make_shared<Camera>();
}

void Application::Initialize() {
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	//�E�B���h�E
	window->InitWindow();

	//�R�}���h����
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
	pmd->Load("PMD/miku/�����~�N.pmd");
	//pmd->Load("PMD/neru/���k�l��.pmd");

	//VMD
	vmd->Load("Motion/swing2.vmd");

	//���_�o�b�t�@
	vertex->Initialize(device->GetDevice(), pmd->GetPMDVertex());

	//�C���f�b�N�X�o�b�t�@
	index->Initialize(device->GetDevice(), pmd->GetIndices());


	camera->Initialize(device->GetDevice());
	model->Initialize(device->GetDevice());

	//�[�x�o�b�t�@
	depth->Initialize(device->GetDevice());

	//�V�F�[�_
	shader->Load(root->GetError());

	//�p�C�v���C��
	pipline->Initialize(device->GetDevice(), shader->GetVS(), shader->GetPS(),
		vertex->GetInputDescNum(), vertex->GetInputDesc(), root->GetRootSignature());

	//�r���[�|�[�g
	viewPort->Initialize();
}

void Application::Run() {
	window->Show();
	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		command->GetCommandAllocator()->Reset();
		command->GetCommandList()->Reset(command->GetCommandAllocator(), pipline->GetPiplineState());

		command->GetCommandList()->SetGraphicsRootSignature(root->GetRootSignature());

		command->GetCommandList()->SetPipelineState(pipline->GetPiplineState());

		command->GetCommandList()->RSSetViewports(1, &viewPort->GetViewPort());

		command->GetCommandList()->RSSetScissorRects(1, &window->GetScissorRect());

		command->GetCommandList()->ResourceBarrier(
			1,
			&CD3DX12_RESOURCE_BARRIER::Transition(
				renderTarget->GetRenderTarget()[swapChain->GetSwapChain()->GetCurrentBackBufferIndex()],
				D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_PRESENT,
				D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_RENDER_TARGET)
		);

		auto bbIndex = swapChain->GetSwapChain()->GetCurrentBackBufferIndex();

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(descriptor->GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(),
			bbIndex, descriptor->GetDescriptorSize());
		command->GetCommandList()->OMSetRenderTargets(1, &rtvHandle, false, &depth->GetHeap()->GetCPUDescriptorHandleForHeapStart());

		const FLOAT color[] = { 0.4f, 0.4f, 0.4f, 1.0f };

		command->GetCommandList()->ClearRenderTargetView(rtvHandle, color, 0, nullptr);

		command->GetCommandList()->IASetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		command->GetCommandList()->IASetVertexBuffers(0, 1, &vertex->GetVBV());

		command->GetCommandList()->IASetIndexBuffer(&index->GetIndexBufferView());

		depth->SetDescriptor(command->GetCommandList());

		camera->UpdateWVP();
		camera->SetDescriptor(command->GetCommandList(), device->GetDevice());

		model->Draw(command->GetCommandList(), device->GetDevice());

		command->GetCommandList()->ResourceBarrier(
			1,
			&CD3DX12_RESOURCE_BARRIER::Transition(
				renderTarget->GetRenderTarget()[swapChain->GetSwapChain()->GetCurrentBackBufferIndex()],
				D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_RENDER_TARGET,
				D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_PRESENT)
		);

		command->GetCommandList()->Close();

		command->Execute();

		swapChain->GetSwapChain()->Present(1, 0);
		command->GetCommandQueue()->Signal(fence->GetFence(), fence->GetFenceValue(true));
		while (fence->GetFence()->GetCompletedValue() != fence->GetFenceValue()) {
		}

	}
}

void Application::Terminate() {
	CoUninitialize();
}

Application::~Application() {
}
