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
//#include "BmpLoader.h"
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
	//bmp				= std::make_shared<BmpLoader>();
	shader			= std::make_shared<ShaderLoader>();
	pipline			= std::make_shared<PipelineState>();
	viewPort		= std::make_shared<ViewPort>();
	cb				= std::make_shared<ConstantBuffer>();
	pmd				= std::make_shared<PMDLoader>();
	index			= std::make_shared<Index>();
	depth			= std::make_shared<DepthStencilBuffer>();
}

//初期化
void Application::Initialize() {
	//ウィンドウのメディアを使うために必要
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	//ウィンドウ
	window->InitWindow();

	//コマンド
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
	pmd->Load("PMD/miku/初音ミク.pmd");
	//pmd->Load("PMD/hibari/雲雀Ver1.10.pmd");

	//BMP
	//bmp->Load("PMD/miku/eye2.bmp");

	//頂点バッファ
	vertex->Initialize(device->GetDevice(), pmd->GetPMDVertex());

	//インデックス
	index->Initialize(device->GetDevice(), pmd->GetIndices());

	//テクスチャリソース
	tex->Initialize(device->GetDevice(), pmd->GetBMPSize().width, pmd->GetBMPSize().height);

	//シェーダリソースビュー
	srv->Initialize(device->GetDevice(), tex->GetTextureBuffer(), pmd->GetMaterial().size());

	//コンスタントバッファ
	cb->Initialize(device->GetDevice(), srv->GetTextureHeap());

	//深度バッファ
	depth->Initialize(device->GetDevice(), srv->GetDescriptorHeapDesc());

	//PMD初期化
	pmd->Initialize(device->GetDevice());

	//シェーダ
	shader->Load(root->GetError());

	//パイプラインステートオブジェクト
	pipline->Initialize(device->GetDevice(), shader->GetVS(), shader->GetPS(), 
		vertex->GetInputDescNum(), vertex->GetInputDesc(), root->GetRootSignature());

	//ビューポート
	viewPort->Initialize();
}

//メインループ
void Application::Run() {
	window->Show();
	while(msg.message != WM_QUIT){
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)){
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		
		//命令のクリア
		command->GetCommandAllocator()->Reset();
		command->GetCommandList()->Reset(command->GetCommandAllocator(), pipline->GetPiplineState());

		//ルートシグネチャのセット
		command->GetCommandList()->SetGraphicsRootSignature(root->GetRootSignature());

		//パイプラインのセット
		command->GetCommandList()->SetPipelineState(pipline->GetPiplineState());

		//ビューポートのセット
		command->GetCommandList()->RSSetViewports(1, &viewPort->GetViewPort());

		//シザーのセット
		command->GetCommandList()->RSSetScissorRects(1, &window->GetScissorRect());

		//バリアを張る
		command->GetCommandList()->ResourceBarrier(
			1,
			&CD3DX12_RESOURCE_BARRIER::Transition(
				renderTarget->GetRenderTarget()[swapChain->GetSwapChain()->GetCurrentBackBufferIndex()], 
				D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_PRESENT,
				D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_RENDER_TARGET)
		);

		//画面に色を付ける
		//バックバッファのインデックスを取得
		auto bbIndex = swapChain->GetSwapChain()->GetCurrentBackBufferIndex();

		//レンダーターゲットの指定
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(descriptor->GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(), 
			bbIndex, descriptor->GetDescriptorSize());
		command->GetCommandList()->OMSetRenderTargets(1, &rtvHandle, false, &depth->GetHeap()->GetCPUDescriptorHandleForHeapStart());

		//クリアカラーの設定
		const FLOAT color[] = { 0.0f, 0.0f, 0.0f, 1.0f };

		//レンダーターゲットのクリア
		command->GetCommandList()->ClearRenderTargetView(rtvHandle, color, 0, nullptr);

		//三角ポリゴン描画にする
		command->GetCommandList()->IASetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//頂点バッファのセット
		command->GetCommandList()->IASetVertexBuffers(0, 1, &vertex->GetVBV());

		//インデックスバッファのセット
		command->GetCommandList()->IASetIndexBuffer(&index->GetIndexBufferView());

		//深度バッファのセット
		depth->SetDescriptor(command->GetCommandList());

		//SRV,コンスタントバッファのデスクリプタをセット
		cb->UpDataWVP();
		for (UINT i = 0; i < 2; i++){
			cb->SetDescriptor(command->GetCommandList(), i, srv->GetTextureHeap(), device->GetDevice());
		}
		
		//テクスチャバッファへの書き込み
		tex->WriteToTextureBuffer(pmd->GetBMPData(), pmd->GetTexFlag());

		//PMD描画
		pmd->Draw(command->GetCommandList(), device->GetDevice(), srv->GetTextureHeap());

		//バリアを張る
		command->GetCommandList()->ResourceBarrier(
			1,
			&CD3DX12_RESOURCE_BARRIER::Transition(
				renderTarget->GetRenderTarget()[swapChain->GetSwapChain()->GetCurrentBackBufferIndex()], 
				D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_RENDER_TARGET,
				D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_PRESENT)
		);

		//コマンドリストを閉じる
		command->GetCommandList()->Close();

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
	CoUninitialize();
}

Application::~Application() {
}
