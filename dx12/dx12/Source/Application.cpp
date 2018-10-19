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
#include "Model.h"
#include "Camera.h"

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
	//tex				= std::make_shared<TextureResource>();
	//srv				= std::make_shared<ShaderResourceView>();
	bmp				= std::make_shared<BmpLoader>();
	imageL			= std::make_shared<ImageLoader>();
	shader			= std::make_shared<ShaderLoader>();
	pipline			= std::make_shared<PipelineState>();
	viewPort		= std::make_shared<ViewPort>();
	//cb				= std::make_shared<ConstantBuffer>();
	pmd				= std::make_shared<PMDLoader>(bmp, imageL);
	index			= std::make_shared<Index>();
	depth			= std::make_shared<DepthStencilBuffer>();
	model			= std::make_shared<Model>();
	camera			= std::make_shared<Camera>();
}

void Application::Initialize() {
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	//ウィンドウ
	window->InitWindow();

	//コマンド周り
	command->InitCommand(device->GetDevice());

	//スワップチェイン
	swapChain->InitSwapChain(command->GetCommandQueue(), window->GetHandleWindow());

	//デスクリプタ
	descriptor->InitDescriptor(device->GetDevice());

	//レンダーターゲット
	renderTarget->InitRenderTarget(swapChain->GetSwapChainDesc().BufferCount,
		device->GetDevice(), swapChain->GetSwapChain(), descriptor->GetDescriptorHandle(), descriptor->GetDescriptorSize());

	//サンプラ
	sampler->InitSampler();

	//ルートシグネチャ
	root->InitRootSignature(sampler->GetSamplerDesc(), device->GetDevice(), D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_ALL);

	//フェンス
	fence->InitFence(device->GetDevice());

	//PMD
	pmd->Load("PMD/neru/亞北ネル.pmd");

	//頂点バッファ
	vertex->Initialize(device->GetDevice(), pmd->GetPMDVertex());

	//インデックスバッファ
	index->Initialize(device->GetDevice(), pmd->GetIndices());

	//テクスチャバッファ
	//tex->Initialize(device->GetDevice(), imageL->GetMetaData());

	//シェーダリソースビュー
	//srv->Initialize(device->GetDevice(), tex->GetTextureBuffer(), pmd->GetMaterial().size());

	//定数バッファ
	//cb->Initialize(device->GetDevice(), srv->GetTextureHeap());

	camera->Initialize(device->GetDevice());
	model->Initialize(device->GetDevice(), imageL->GetMetaData(), pmd->GetMaterial(), pmd->GetTextureNum());

	//深度バッファ
	depth->Initialize(device->GetDevice());

	//PMD初期化
	//pmd->Initialize(device->GetDevice());

	//シェーダ
	shader->Load(root->GetError());

	//パイプライン
	pipline->Initialize(device->GetDevice(), shader->GetVS(), shader->GetPS(),
		vertex->GetInputDescNum(), vertex->GetInputDesc(), root->GetRootSignature());

	//ビューポート
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

		//
		//cb->UpDataWVP();
		//for (UINT i = 0; i < 2; i++) {
		//	cb->SetDescriptor(command->GetCommandList(), i, srv->GetTextureHeap(), device->GetDevice());
		//}

		camera->UpdateWVP();
		camera->SetDescriptor(command->GetCommandList(), device->GetDevice());

		//
		//tex->WriteToTextureBuffer(imageL->GetMetaData(), imageL->GetScratchImage(), pmd->GetTexFlag());
		model->WriteToTextureBuffer(imageL->GetMetaData(), imageL->GetScratchImage(), pmd->GetTexFlag());

		//
		//pmd->Draw(command->GetCommandList(), device->GetDevice(), srv->GetTextureHeap());
		model->Draw(command->GetCommandList(), device->GetDevice(), pmd->GetMaterial());
		//command->GetCommandList()->DrawIndexedInstanced(pmd->GetIndices().size(), 1, 0, 0, 0);

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
