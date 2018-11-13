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
	//1�p�X��
	renderTarget->Init1stPathRTVSRV(device->GetDevice());
	//2�p�X��
	renderTarget->Init2ndPathRTVSRV(device->GetDevice());

	//�T���v��
	sampler->InitSampler();

	//���[�g�V�O�l�`��
	root->InitRootSignature(sampler->GetSamplerDesc(), device->GetDevice(), D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_ALL);

	//�t�F���X
	fence->InitFence(device->GetDevice());

	//PMD
	//pmd->Load("PMD/miku/�����~�N.pmd");
	//pmd->Load("PMD/miku/�����~�Nmetal.pmd");
	//pmd->Load("PMD/neru/���k�l��.pmd");
	//pmd->Load("PMD/reimu/reimu_F01.pmd");
	//pmd->Load("PMD/luka/�������J.pmd");
	//pmd->Load("PMD/hibiki/��ߔe��v1_�O���r�A�~�Y�M.pmd");
	pmd->Load("PMD/asuka/��Ver1.10.pmd");
	//pmd->Load("PMD/yagyu/����Ver1.12.pmd");
	//pmd->Load("PMD/katuragi/����Ver1.10.pmd");
	//pmd->Load("PMD/ikaruga/����Ver1.10.pmd");
	//pmd->Load("PMD/hibari/�_��Ver1.10.pmd");

	//VMD
	//vmd->Load("Motion/swing2.vmd");
	vmd->Load("Motion/���S�R���_���X.vmd");
	//vmd->Load("Motion/���ꂭ�Ƃ�������񂶂���.vmd");

	//���_�o�b�t�@
	vertex->Initialize(device->GetDevice(), pmd->GetPMDVertex());

	//�C���f�b�N�X�o�b�t�@
	index->Initialize(device->GetDevice(), pmd->GetIndices());

	//�J����
	camera->Initialize(device->GetDevice());

	//���f��
	model->Initialize(device->GetDevice());

	//�[�x�o�b�t�@
	depth->Initialize(device->GetDevice());

	//�V�F�[�_
	shader->Load(root->GetError());

	//�p�C�v���C��
	pipline->Initialize(device->GetDevice(), shader->GetVS(), shader->GetPS(),
		vertex->GetInputDescNum(), vertex->GetInputDesc(), root->GetRootSignature());
	//1�p�X��
	pipline->PeraInitialize(device->GetDevice(), shader->GetPeraVS(0), shader->GetPeraPS(0),
		vertex->GetPeraInputDescNum(), vertex->GetPeraInputDesc(), root->GetPeraRootSignature()[0], 0);
	//2�p�X��
	pipline->PeraInitialize(device->GetDevice(), shader->GetPeraVS(1), shader->GetPeraPS(1),
		vertex->GetPeraInputDescNum(), vertex->GetPeraInputDesc(), root->GetPeraRootSignature()[1], 1);

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

		//���܂ł̃����_�����O

		auto r = command->GetCommandAllocator()->Reset();

		command->GetCommandList()->Reset(command->GetCommandAllocator(), pipline->GetPiplineState());

		command->GetCommandList()->SetGraphicsRootSignature(root->GetRootSignature());

		command->GetCommandList()->RSSetViewports(1, &viewPort->GetViewPort());

		command->GetCommandList()->RSSetScissorRects(1, &window->GetScissorRect());

		//�o���A
		command->GetCommandList()->ResourceBarrier(
			1,
			&CD3DX12_RESOURCE_BARRIER::Transition(
				renderTarget->GetPeraRenderTarget(),
				D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_PRESENT,
				D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_RENDER_TARGET)
		);

		//�����_�[�^�[�Q�b�g�̃Z�b�g
		auto rtvHandle = renderTarget->GetHeap()["RTV"]->GetCPUDescriptorHandleForHeapStart();
		command->GetCommandList()->OMSetRenderTargets(1, &rtvHandle, false, &depth->GetHeap()->GetCPUDescriptorHandleForHeapStart());

		const FLOAT color[] = { 0.4f, 0.4f, 0.4f, 1.0f };

		command->GetCommandList()->ClearRenderTargetView(rtvHandle, color, 0, nullptr);

		command->GetCommandList()->IASetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		command->GetCommandList()->IASetVertexBuffers(0, 1, &vertex->GetVBV());

		command->GetCommandList()->IASetIndexBuffer(&index->GetIndexBufferView());

		depth->SetDescriptor(command->GetCommandList());

		camera->UpdateWVP();
		//
		camera->SetDescriptor(command->GetCommandList(), device->GetDevice());

		model->Update();
		//
		model->Draw(command->GetCommandList(), device->GetDevice());

		//���f���`��֘A���o���h��
		//CreateModelDrawBundle();

		//�o���A
		command->GetCommandList()->ResourceBarrier(
			1,
			&CD3DX12_RESOURCE_BARRIER::Transition(
				renderTarget->GetPeraRenderTarget(),
				D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_RENDER_TARGET,
				D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_PRESENT)
		);

		//command->GetCommandList()->ExecuteBundle(command->GetBundleCommandList());

		command->GetCommandList()->Close();

		command->Execute();

		command->GetCommandQueue()->Signal(fence->GetFence(), fence->GetFenceValue(true));
		while (fence->GetFence()->GetCompletedValue() != fence->GetFenceValue()) {
		}

		//�y���|���S��
		//1�p�X��
		UpdatePera();
		//2�p�X��
		//UpdatePera2nd();

		swapChain->GetSwapChain()->Present(0, 0);
	}
}

void Application::Terminate() {
	CoUninitialize();
}

//�y���|���S���p
void Application::UpdatePera() {
	//�A���P�[�^�̃��Z�b�g
	command->GetCommandAllocator()->Reset();

	//�R�}���h���X�g�̃��Z�b�g
	command->GetCommandList()->Reset(command->GetCommandAllocator(), pipline->GetPeraPiplineState()[0]);

	//���[�g�V�O�l�`���̃Z�b�g
	command->GetCommandList()->SetGraphicsRootSignature(root->GetPeraRootSignature()[0]);

	//�r���[�|�[�g�̃Z�b�g
	command->GetCommandList()->RSSetViewports(1, &viewPort->GetViewPort());

	//�V�U�[���N�g�̃Z�b�g
	command->GetCommandList()->RSSetScissorRects(1, &window->GetScissorRect());

	auto bbIndex = swapChain->GetSwapChain()->GetCurrentBackBufferIndex();

	//�����_�����O
	command->GetCommandList()->ResourceBarrier(
		1,
		&CD3DX12_RESOURCE_BARRIER::Transition(
			renderTarget->GetRenderTarget()[bbIndex],
			//renderTarget->GetPeraRenderTarget2(),
			D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_PRESENT,
			D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_RENDER_TARGET)
	);

	//auto rtvHandle = renderTarget->GetHeap2nd()["RTV"]->GetCPUDescriptorHandleForHeapStart();
	//command->GetCommandList()->OMSetRenderTargets(1, &rtvHandle, false, &depth->GetHeap()->GetCPUDescriptorHandleForHeapStart());
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(descriptor->GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(),
		bbIndex, descriptor->GetDescriptorSize());

	command->GetCommandList()->OMSetRenderTargets(1, &rtvHandle, false, &depth->GetHeap()->GetCPUDescriptorHandleForHeapStart());

	const FLOAT color[] = { 0.4f, 0.4f, 0.4f, 1.0f };

	command->GetCommandList()->ClearRenderTargetView(rtvHandle, color, 0, nullptr);

	//SRV�̃q�[�v�Z�b�g
	auto srvH = renderTarget->GetHeap()["SRV"];
	command->GetCommandList()->SetDescriptorHeaps(1, &srvH);

	command->GetCommandList()->SetGraphicsRootDescriptorTable(0, srvH->GetGPUDescriptorHandleForHeapStart());

	//�v���~�e�B�u�g�|���W�[
	command->GetCommandList()->IASetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//�y���|���S����VBV���Z�b�g
	command->GetCommandList()->IASetVertexBuffers(0, 1, &vertex->GetPeraVBV(0));

	//�`��
	command->GetCommandList()->DrawInstanced(4, 1, 0, 0);

	//�o���A
	command->GetCommandList()->ResourceBarrier(
		1,
		&CD3DX12_RESOURCE_BARRIER::Transition(
			renderTarget->GetRenderTarget()[bbIndex],
			//renderTarget->GetPeraRenderTarget2(),
			D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_PRESENT)
	);

	//�R�}���h���X�g�����
	command->GetCommandList()->Close();

	//���s
	command->Execute();

	command->GetCommandQueue()->Signal(fence->GetFence(), fence->GetFenceValue(true));

	//�ҋ@
	while (fence->GetFence()->GetCompletedValue() != fence->GetFenceValue()) {
	}
}

void Application::UpdatePera2nd() {
	//�A���P�[�^�̃��Z�b�g
	command->GetCommandAllocator()->Reset();

	//�R�}���h���X�g�̃��Z�b�g
	command->GetCommandList()->Reset(command->GetCommandAllocator(), pipline->GetPeraPiplineState()[1]);

	//���[�g�V�O�l�`���̃Z�b�g
	command->GetCommandList()->SetGraphicsRootSignature(root->GetPeraRootSignature()[1]);

	//�r���[�|�[�g�̃Z�b�g
	command->GetCommandList()->RSSetViewports(1, &viewPort->GetViewPort());

	//�V�U�[���N�g�̃Z�b�g
	command->GetCommandList()->RSSetScissorRects(1, &window->GetScissorRect());

	auto bbIndex = swapChain->GetSwapChain()->GetCurrentBackBufferIndex();

	//�����_�����O
	command->GetCommandList()->ResourceBarrier(
		1,
		&CD3DX12_RESOURCE_BARRIER::Transition(
			renderTarget->GetRenderTarget()[bbIndex],
			D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_PRESENT,
			D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_RENDER_TARGET)
	);

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(descriptor->GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(),
		bbIndex, descriptor->GetDescriptorSize());

	command->GetCommandList()->OMSetRenderTargets(1, &rtvHandle, false, &depth->GetHeap()->GetCPUDescriptorHandleForHeapStart());

	const FLOAT color[] = { 0.4f, 0.4f, 0.4f, 1.0f };

	command->GetCommandList()->ClearRenderTargetView(rtvHandle, color, 0, nullptr);

	//SRV�̃q�[�v�Z�b�g
	auto srvH = renderTarget->GetHeap2nd()["SRV"];
	command->GetCommandList()->SetDescriptorHeaps(1, &srvH);

	command->GetCommandList()->SetGraphicsRootDescriptorTable(0, srvH->GetGPUDescriptorHandleForHeapStart());

	//�v���~�e�B�u�g�|���W�[
	command->GetCommandList()->IASetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//�y���|���S����VBV���Z�b�g
	command->GetCommandList()->IASetVertexBuffers(0, 1, &vertex->GetPeraVBV(1));

	//�`��
	command->GetCommandList()->DrawInstanced(4, 1, 0, 0);

	//�o���A
	command->GetCommandList()->ResourceBarrier(
		1,
		&CD3DX12_RESOURCE_BARRIER::Transition(
			renderTarget->GetRenderTarget()[bbIndex],
			D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_PRESENT)
	);

	//�R�}���h���X�g�����
	command->GetCommandList()->Close();

	//���s
	command->Execute();

	command->GetCommandQueue()->Signal(fence->GetFence(), fence->GetFenceValue(true));

	//�ҋ@
	while (fence->GetFence()->GetCompletedValue() != fence->GetFenceValue()) {
	}
}

void Application::CreateModelDrawBundle() {
	command->CreateModelDrawBundle(device->GetDevice(), pipline->GetPiplineState());

	command->GetBundleCommandList()->SetGraphicsRootSignature(root->GetRootSignature());

	command->GetBundleCommandList()->IASetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	command->GetBundleCommandList()->IASetVertexBuffers(0, 1, &vertex->GetVBV());
	command->GetBundleCommandList()->IASetIndexBuffer(&index->GetIndexBufferView());

	camera->SetDescriptor(command->GetBundleCommandList(), device->GetDevice());
	model->Draw(command->GetBundleCommandList(), device->GetDevice());

	command->GetBundleCommandList()->Close();
}

Application::~Application() {
}
