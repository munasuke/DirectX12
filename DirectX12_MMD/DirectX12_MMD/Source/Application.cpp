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
	//1パス目
	renderTarget->Init1stPathRTVSRV(device->GetDevice());
	//2パス目
	renderTarget->Init2ndPathRTVSRV(device->GetDevice());

	//サンプラ
	sampler->InitSampler();

	//ルートシグネチャ
	root->InitRootSignature(sampler->GetSamplerDesc(), device->GetDevice(), D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_ALL);

	//フェンス
	fence->InitFence(device->GetDevice());

	//PMD
	//pmd->Load("PMD/miku/初音ミク.pmd");
	//pmd->Load("PMD/miku/初音ミクmetal.pmd");
	//pmd->Load("PMD/neru/亞北ネル.pmd");
	//pmd->Load("PMD/reimu/reimu_F01.pmd");
	//pmd->Load("PMD/luka/巡音ルカ.pmd");
	//pmd->Load("PMD/hibiki/我那覇響v1_グラビアミズギ.pmd");
	pmd->Load("PMD/asuka/飛鳥Ver1.10.pmd");
	//pmd->Load("PMD/yagyu/柳生Ver1.12.pmd");
	//pmd->Load("PMD/katuragi/葛城Ver1.10.pmd");
	//pmd->Load("PMD/ikaruga/斑鳩Ver1.10.pmd");
	//pmd->Load("PMD/hibari/雲雀Ver1.10.pmd");

	//VMD
	//vmd->Load("Motion/swing2.vmd");
	vmd->Load("Motion/ヤゴコロダンス.vmd");
	//vmd->Load("Motion/えれくとりっくえんじぇぅ.vmd");

	//頂点バッファ
	vertex->Initialize(device->GetDevice(), pmd->GetPMDVertex());

	//インデックスバッファ
	index->Initialize(device->GetDevice(), pmd->GetIndices());

	//カメラ
	camera->Initialize(device->GetDevice());

	//モデル
	model->Initialize(device->GetDevice());

	//深度バッファ
	depth->Initialize(device->GetDevice());

	//シェーダ
	shader->Load(root->GetError());

	//パイプライン
	pipline->Initialize(device->GetDevice(), shader->GetVS(), shader->GetPS(),
		vertex->GetInputDescNum(), vertex->GetInputDesc(), root->GetRootSignature());
	//1パス目
	pipline->PeraInitialize(device->GetDevice(), shader->GetPeraVS(0), shader->GetPeraPS(0),
		vertex->GetPeraInputDescNum(), vertex->GetPeraInputDesc(), root->GetPeraRootSignature()[0], 0);
	//2パス目
	pipline->PeraInitialize(device->GetDevice(), shader->GetPeraVS(1), shader->GetPeraPS(1),
		vertex->GetPeraInputDescNum(), vertex->GetPeraInputDesc(), root->GetPeraRootSignature()[1], 1);

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

		//今までのレンダリング

		auto r = command->GetCommandAllocator()->Reset();

		command->GetCommandList()->Reset(command->GetCommandAllocator(), pipline->GetPiplineState());

		command->GetCommandList()->SetGraphicsRootSignature(root->GetRootSignature());

		command->GetCommandList()->RSSetViewports(1, &viewPort->GetViewPort());

		command->GetCommandList()->RSSetScissorRects(1, &window->GetScissorRect());

		//バリア
		command->GetCommandList()->ResourceBarrier(
			1,
			&CD3DX12_RESOURCE_BARRIER::Transition(
				renderTarget->GetPeraRenderTarget(),
				D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_PRESENT,
				D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_RENDER_TARGET)
		);

		//レンダーターゲットのセット
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

		//モデル描画関連をバンドル
		//CreateModelDrawBundle();

		//バリア
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

		//ペラポリゴン
		//1パス目
		UpdatePera();
		//2パス目
		//UpdatePera2nd();

		swapChain->GetSwapChain()->Present(0, 0);
	}
}

void Application::Terminate() {
	CoUninitialize();
}

//ペラポリゴン用
void Application::UpdatePera() {
	//アロケータのリセット
	command->GetCommandAllocator()->Reset();

	//コマンドリストのリセット
	command->GetCommandList()->Reset(command->GetCommandAllocator(), pipline->GetPeraPiplineState()[0]);

	//ルートシグネチャのセット
	command->GetCommandList()->SetGraphicsRootSignature(root->GetPeraRootSignature()[0]);

	//ビューポートのセット
	command->GetCommandList()->RSSetViewports(1, &viewPort->GetViewPort());

	//シザーレクトのセット
	command->GetCommandList()->RSSetScissorRects(1, &window->GetScissorRect());

	auto bbIndex = swapChain->GetSwapChain()->GetCurrentBackBufferIndex();

	//レンダリング
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

	//SRVのヒープセット
	auto srvH = renderTarget->GetHeap()["SRV"];
	command->GetCommandList()->SetDescriptorHeaps(1, &srvH);

	command->GetCommandList()->SetGraphicsRootDescriptorTable(0, srvH->GetGPUDescriptorHandleForHeapStart());

	//プリミティブトポロジー
	command->GetCommandList()->IASetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//ペラポリゴンのVBVをセット
	command->GetCommandList()->IASetVertexBuffers(0, 1, &vertex->GetPeraVBV(0));

	//描画
	command->GetCommandList()->DrawInstanced(4, 1, 0, 0);

	//バリア
	command->GetCommandList()->ResourceBarrier(
		1,
		&CD3DX12_RESOURCE_BARRIER::Transition(
			renderTarget->GetRenderTarget()[bbIndex],
			//renderTarget->GetPeraRenderTarget2(),
			D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_PRESENT)
	);

	//コマンドリストを閉じる
	command->GetCommandList()->Close();

	//実行
	command->Execute();

	command->GetCommandQueue()->Signal(fence->GetFence(), fence->GetFenceValue(true));

	//待機
	while (fence->GetFence()->GetCompletedValue() != fence->GetFenceValue()) {
	}
}

void Application::UpdatePera2nd() {
	//アロケータのリセット
	command->GetCommandAllocator()->Reset();

	//コマンドリストのリセット
	command->GetCommandList()->Reset(command->GetCommandAllocator(), pipline->GetPeraPiplineState()[1]);

	//ルートシグネチャのセット
	command->GetCommandList()->SetGraphicsRootSignature(root->GetPeraRootSignature()[1]);

	//ビューポートのセット
	command->GetCommandList()->RSSetViewports(1, &viewPort->GetViewPort());

	//シザーレクトのセット
	command->GetCommandList()->RSSetScissorRects(1, &window->GetScissorRect());

	auto bbIndex = swapChain->GetSwapChain()->GetCurrentBackBufferIndex();

	//レンダリング
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

	//SRVのヒープセット
	auto srvH = renderTarget->GetHeap2nd()["SRV"];
	command->GetCommandList()->SetDescriptorHeaps(1, &srvH);

	command->GetCommandList()->SetGraphicsRootDescriptorTable(0, srvH->GetGPUDescriptorHandleForHeapStart());

	//プリミティブトポロジー
	command->GetCommandList()->IASetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//ペラポリゴンのVBVをセット
	command->GetCommandList()->IASetVertexBuffers(0, 1, &vertex->GetPeraVBV(1));

	//描画
	command->GetCommandList()->DrawInstanced(4, 1, 0, 0);

	//バリア
	command->GetCommandList()->ResourceBarrier(
		1,
		&CD3DX12_RESOURCE_BARRIER::Transition(
			renderTarget->GetRenderTarget()[bbIndex],
			D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_PRESENT)
	);

	//コマンドリストを閉じる
	command->GetCommandList()->Close();

	//実行
	command->Execute();

	command->GetCommandQueue()->Signal(fence->GetFence(), fence->GetFenceValue(true));

	//待機
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
