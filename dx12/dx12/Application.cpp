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

namespace {
	MSG msg = {};
}

Application::Application() {
	window = std::make_shared<Window>();
	device = std::make_shared<Device>();
	command = std::make_shared<Command>();
	swapChain = std::make_shared<SwapChain>();
	descriptor = std::make_shared<Descriptor>();
	renderTarget = std::make_shared<RenderTarget>();
	sampler = std::make_shared<Sampler>();
	root = std::make_shared<RootSignature>();
	fence = std::make_shared<Fence>();
	vertex = std::make_shared<Vertex>();
	tex = std::make_shared<TextureResource>();
	srv = std::make_shared<ShaderResourceView>();
	bmp = std::make_shared<BmpLoader>();
	imageL = std::make_shared<ImageLoader>();
	shader = std::make_shared<ShaderLoader>();
	pipline = std::make_shared<PipelineState>();
	viewPort = std::make_shared<ViewPort>();
	cb = std::make_shared<ConstantBuffer>();
	pmd = std::make_shared<PMDLoader>(bmp, imageL);
	index = std::make_shared<Index>();
	depth = std::make_shared<DepthStencilBuffer>();
}

void Application::Initialize() {
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	window->InitWindow();

	command->InitCommand(device->GetDevice());

	swapChain->InitSwapChain(command->GetCommandQueue(), window->GetHandleWindow());

	descriptor->InitDescriptor(device->GetDevice());

	renderTarget->InitRenderTarget(swapChain->GetSwapChainDesc().BufferCount,
		device->GetDevice(), swapChain->GetSwapChain(), descriptor->GetDescriptorHandle(), descriptor->GetDescriptorSize());

	sampler->InitSampler();

	root->InitRootSignature(sampler->GetSamplerDesc(), device->GetDevice(), D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_ALL);

	fence->InitFence(device->GetDevice());

	pmd->Load("PMD/miku/‰‰¹ƒ~ƒN.pmd");

	vertex->Initialize(device->GetDevice(), pmd->GetPMDVertex());

	index->Initialize(device->GetDevice(), pmd->GetIndices());

	tex->Initialize(device->GetDevice(), imageL->GetMetaData());

	srv->Initialize(device->GetDevice(), tex->GetTextureBuffer(), pmd->GetMaterial().size());

	cb->Initialize(device->GetDevice(), srv->GetTextureHeap());

	depth->Initialize(device->GetDevice(), srv->GetDescriptorHeapDesc());

	pmd->Initialize(device->GetDevice());

	shader->Load(root->GetError());

	pipline->Initialize(device->GetDevice(), shader->GetVS(), shader->GetPS(),
		vertex->GetInputDescNum(), vertex->GetInputDesc(), root->GetRootSignature());

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

		const FLOAT color[] = { 0.0f, 0.0f, 0.0f, 1.0f };

		command->GetCommandList()->ClearRenderTargetView(rtvHandle, color, 0, nullptr);

		command->GetCommandList()->IASetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		command->GetCommandList()->IASetVertexBuffers(0, 1, &vertex->GetVBV());

		command->GetCommandList()->IASetIndexBuffer(&index->GetIndexBufferView());

		depth->SetDescriptor(command->GetCommandList());

		cb->UpDataWVP();
		for (UINT i = 0; i < 2; i++) {
			cb->SetDescriptor(command->GetCommandList(), i, srv->GetTextureHeap(), device->GetDevice());
		}

		tex->WriteToTextureBuffer(imageL->GetMetaData(), imageL->GetScratchImage(), pmd->GetTexFlag());

		pmd->Draw(command->GetCommandList(), device->GetDevice(), srv->GetTextureHeap());

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
