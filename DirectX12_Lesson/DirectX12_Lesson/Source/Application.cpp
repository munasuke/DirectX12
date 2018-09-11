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

//初期化
void Application::Initialize() {
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
	root->InitRootSignature(sampler->GetSamplerDesc(), device->GetDevice(), D3D12_SHADER_VISIBILITY_ALL);
	//フェンス
	fence->InitFence(device->GetDevice());
	//頂点バッファ
	vertex->Initialize(device->GetDevice());
	//テクスチャリソース
	tex->Initialize(device->GetDevice());
	//シェーダリソースビュー
	srv->Initialize(device->GetDevice(), tex->GetTextureBuffer());
	//BMP
	bmp->Load("Image/aoba.bmp");
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
		D3D12_RECT scissorRect = { 0, 0, WIN_WIDTH, WIN_HEIGHT };
		command->GetCommandList()->RSSetScissorRects(1, &window->GetScissorRect());

		//SRV用のデスクリプタをセット
		command->GetCommandList()->SetDescriptorHeaps(1, srv->GetTextureHeap2());
		command->GetCommandList()->SetGraphicsRootDescriptorTable(0, srv->GetTextureHeap()->GetGPUDescriptorHandleForHeapStart());

		//バリアを張る
		command->GetCommandList()->ResourceBarrier(
			0,
			&CD3DX12_RESOURCE_BARRIER::Transition(
				renderTarget->GetRenderTarget()[swapChain->GetSwapChain()->GetCurrentBackBufferIndex()], 
				D3D12_RESOURCE_STATE_PRESENT, 
				D3D12_RESOURCE_STATE_RENDER_TARGET)
		);

		//頂点バッファのセット
		command->GetCommandList()->IASetVertexBuffers(0, 1, &vertex->GetVBV());

		//画面に色を付ける
		//バックバッファのインデックスを取得
		auto bbIndex = swapChain->GetSwapChain()->GetCurrentBackBufferIndex();

		//レンダーターゲットの指定
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(descriptor->GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(), 
			bbIndex, descriptor->GetDescriptorSize());
		command->GetCommandList()->OMSetRenderTargets(1, &rtvHandle, false, nullptr);

		//クリアカラーの設定
		const FLOAT color[] = { 0.0f, 0.0f, 0.0f, 1.0f };

		//レンダーターゲットのクリア
		command->GetCommandList()->ClearRenderTargetView(rtvHandle, color, 0, nullptr);

		//三角ポリゴン描画にする
		command->GetCommandList()->IASetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//テクスチャバッファへの書き込み
		tex->WriteToTextureBuffer(bmp->GetData());

		//頂点描画
		command->GetCommandList()->DrawInstanced(6, 1, 0, 0);

		//バリアを張る
		command->GetCommandList()->ResourceBarrier(
			0,
			&CD3DX12_RESOURCE_BARRIER::Transition(
				tex->GetTextureBuffer(),
				D3D12_RESOURCE_STATE_COPY_DEST,
				D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE)
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
}

Application::~Application() {
}
