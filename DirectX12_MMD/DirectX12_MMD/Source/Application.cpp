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
#include "PrimitiveCreator.h"
#include "ShadowMap.h"

#include <DirectXMath.h>

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

	//プリミティブ
	prim = std::make_shared<PrimitiveManager>(device->GetDevice());
	//平面
	auto color = DirectX::XMFLOAT4(0.4f, 0.8f, 0.4f, 1.0f);
	prim->CreatePlane(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), 50.0f, 50.0f, color);
	//円錐
	color = DirectX::XMFLOAT4(0.8f, 0.4f, 0.4f, 1.0f);
	prim->CreateCone(DirectX::XMFLOAT3(15.0f, 0.0f, 5.0f), 100, 4.0f, 15.0f, color);
	color = DirectX::XMFLOAT4(0.4f, 0.4f, 0.8f, 1.0f);
	prim->CreateCone(DirectX::XMFLOAT3(-10.0f, 0.0f, 15.0f), 100, 4.0f, 10.0f, color);

	//シャドウマップ
	shadowMap.reset(new ShadowMap(device->GetDevice(), vertex->GetInputDescNum(), vertex->GetInputDesc()));

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
		//アロケータのセット
		command->GetCommandAllocator()->Reset();

		auto list = command->GetCommandList();
		
		auto r =list->Reset(command->GetCommandAllocator(), shadowMap->GetGps());

		list->SetGraphicsRootSignature(shadowMap->GetRs());

		auto viewP = viewPort->GetViewPort();
		auto shadowDesc = shadowMap->GetBuffer()->GetDesc();
		viewP.Height	= shadowDesc.Height;
		viewP.Width		= shadowDesc.Width;
		list->RSSetViewports(1, &viewP);

		auto scissor = window->GetScissorRect();
		scissor.bottom	= shadowDesc.Height;
		scissor.right	= shadowDesc.Width;
		list->RSSetScissorRects(1, &scissor);

		//バリア
		list->ResourceBarrier(
			1,
			&CD3DX12_RESOURCE_BARRIER::Transition(
				shadowMap->GetBuffer(),
				D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_PRESENT,
				D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_DEPTH_WRITE)
		);

		//トポロジー
		list->IASetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//モデルの頂点バッファのセット
		list->IASetVertexBuffers(0, 1, &vertex->GetVBV());

		//モデルのインデックスバッファのセット
		list->IASetIndexBuffer(&index->GetIndexBufferView());

		//シャドウマップの準備
		shadowMap->Setup(list, model->GetBoneBuffer());
		//ライトビュー用カメラのセット
		camera->SetDescriptor(list, device->GetDevice());
		//シャドウマップの描画
		shadowMap->Draw(list, pmd->GetIndices().size());

		list->ResourceBarrier(
			1,
			&CD3DX12_RESOURCE_BARRIER::Transition(
				shadowMap->GetBuffer(),
				D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_DEPTH_WRITE,
				D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_PRESENT)
		);

		list->SetPipelineState(pipline->GetPiplineState());

		list->SetGraphicsRootSignature(root->GetRootSignature());

		auto pera = renderTarget->GetPeraRenderTarget()->GetDesc();
		viewP.Height	= pera.Height;
		viewP.Width		= pera.Width;
		list->RSSetViewports(1, &viewP);
		//list->RSSetViewports(1, &viewPort->GetViewPort());

		scissor.bottom	= pera.Height;
		scissor.right	= pera.Width;
		list->RSSetScissorRects(1, &scissor);
		//list->RSSetScissorRects(1, &window->GetScissorRect());

		//バリア
		list->ResourceBarrier(
			1,
			&CD3DX12_RESOURCE_BARRIER::Transition(
				renderTarget->GetPeraRenderTarget(),
				D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_PRESENT,
				D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_RENDER_TARGET)
		);

		//レンダーターゲットのセット
		auto rtvHandle = renderTarget->GetHeap()["RTV"]->GetCPUDescriptorHandleForHeapStart();
		list->OMSetRenderTargets(1, &rtvHandle, false, &depth->GetHeap()->GetCPUDescriptorHandleForHeapStart());

		const FLOAT color[] = { 0.4f, 0.4f, 0.4f, 1.0f };

		D3D12_RECT rec = {};
		rec.top		= 0;
		rec.bottom	= WIN_HEIGHT;
		rec.left	= 0;
		rec.right	= WIN_WIDTH;
		list->ClearRenderTargetView(rtvHandle, color, 1, &rec);


		//トポロジー
		list->IASetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//モデルの頂点バッファのセット
		list->IASetVertexBuffers(0, 1, &vertex->GetVBV());

		//モデルのインデックスバッファのセット
		list->IASetIndexBuffer(&index->GetIndexBufferView());

		depth->SetDescriptor(list);

		//camera->UpdateWVP();
		//
		camera->SetDescriptor(list, device->GetDevice());

		model->Update();
		//
		model->Draw(list, device->GetDevice());

		//プリミティブの描画準備
		prim->SetPrimitiveDrawMode(list);
		//カメラの再セット
		camera->SetDescriptor(list, device->GetDevice());
		//シャドウマップのセット
		auto srvH = shadowMap->GetHeap()["SRV"];
		command->GetCommandList()->SetDescriptorHeaps(1, &srvH);
		command->GetCommandList()->SetGraphicsRootDescriptorTable(1, srvH->GetGPUDescriptorHandleForHeapStart());
		//プリミティブの描画
		prim->Draw(list);

		//モデル描画関連をバンドル
		//CreateModelDrawBundle();

		//バリア
		list->ResourceBarrier(
			1,
			&CD3DX12_RESOURCE_BARRIER::Transition(
				renderTarget->GetPeraRenderTarget(),
				D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_RENDER_TARGET,
				D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_PRESENT)
		);

		//list->ExecuteBundle(command->GetBundleCommandList());

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
//1パス目の処理
void Application::UpdatePera() {
	//パイプラインのセット
	command->GetCommandList()->SetPipelineState(pipline->GetPeraPiplineState()[0]);

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
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(descriptor->GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(),
		bbIndex, descriptor->GetDescriptorSize());

	command->GetCommandList()->OMSetRenderTargets(1, &rtvHandle, false, nullptr);

	const FLOAT color[] = { 1.0f, 0.0f, 0.0f, 1.0f };

	D3D12_RECT rec = {};
	rec.top		= 0;
	rec.bottom	= WIN_HEIGHT;
	rec.left	= 0;
	rec.right	= WIN_WIDTH;
	command->GetCommandList()->ClearRenderTargetView(rtvHandle, color, 0, &rec);
	
	//SRVのヒープセット
	auto srvH = renderTarget->GetHeap()["SRV"];
	command->GetCommandList()->SetDescriptorHeaps(1, &srvH);
	command->GetCommandList()->SetGraphicsRootDescriptorTable(0, srvH->GetGPUDescriptorHandleForHeapStart());

	auto srvH2 = shadowMap->GetHeap()["SRV"];
	command->GetCommandList()->SetDescriptorHeaps(1, &srvH2);
	command->GetCommandList()->SetGraphicsRootDescriptorTable(1, srvH2->GetGPUDescriptorHandleForHeapStart());

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
	auto r = command->GetCommandList()->Close();

	//実行
	command->Execute();

	command->GetCommandQueue()->Signal(fence->GetFence(), fence->GetFenceValue(true));

	//待機
	while (fence->GetFence()->GetCompletedValue() != fence->GetFenceValue()) {
	}
}

//2パス目の処理
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

	const FLOAT color[] = { 0.0f, 1.0f, 0.0f, 1.0f };

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

	//待機
	command->GetCommandQueue()->Signal(fence->GetFence(), fence->GetFenceValue(true));
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
