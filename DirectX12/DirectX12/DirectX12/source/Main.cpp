#include <Windows.h>	//ウィンドウ表示するのに必要
#include <d3d12.h>		//DirectX12を使うのに必要
#include "d3dx12.h"		//DirectX12を使いやすくするヘッダ
#include <dxgi1_4.h>	//DXGIを扱うのに必要(DX12ではDXGI1.4が使われてる)
#include <D3Dcompiler.h>//シェーダコンパイラで必要
#include <DirectXMath.h>//数学系の便利なのが入ってるヘッダ
#include <tchar.h>
#include <vector>

using namespace DirectX;

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#define WIN_WIDTH	(640)	//ウィンドウサイズ幅
#define WIN_HEIGTH	(480)	//ウィンドウサイズ高

#define Release(X)	{if((X) != nullptr) (X)->Release();}

//プロトタイプ宣言
LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	WNDCLASSEX w = {};
	w.cbSize = sizeof(WNDCLASSEX);
	w.lpfnWndProc = (WNDPROC)WindowProcedure;//コールバック関数の指定
	w.lpszClassName = _T("DirectX12");//アプリケーションクラス名
	w.hInstance = GetModuleHandle(0);//ハンドルの取得
	RegisterClassEx(&w);//アプリケーションクラス

	//ウィンドウサイズ設定
	RECT wrc = { 0, 0, WIN_WIDTH, WIN_HEIGTH };//ウィンドウサイズを設定
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);//補正

	//ウィンドウ生成
	HWND hwnd = CreateWindow(w.lpszClassName,//クラス名指定
		_T("DirectX12"),			//タイトルバーの文字
		WS_OVERLAPPEDWINDOW,		//タイトルバーと境界線があるウィンドウ
		CW_USEDEFAULT,			//表示X座標はOSにお任せ
		CW_USEDEFAULT,			//表示Y座標はOSにお任せ
		wrc.right - wrc.left,		//ウィンドウ幅
		wrc.bottom - wrc.top,		//ウィンドウ高
		nullptr,					//親ウィンドウのハンドル
		nullptr,					//メニューハンドル
		w.hInstance,				//呼び出しアプリケーションハンドル
		nullptr);				//追加パラメータ

	//ウィンドウ表示
	ShowWindow(hwnd, SW_SHOW);

	//初期化
	D3D_FEATURE_LEVEL levels[] = {
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0
	};
	D3D_FEATURE_LEVEL level = {};
	ID3D12Device* dev = nullptr;
	HRESULT result = S_OK;
	//エラーを出力に表示させる
#ifdef _DEBUG
	ID3D12Debug *debug = nullptr;
	result = D3D12GetDebugInterface(IID_PPV_ARGS(&debug));
	if (FAILED(result))
		int i = 0;
	debug->EnableDebugLayer();
	Release(debug);
	debug = nullptr;
#endif
	for (auto i : levels) {	//マシンにあったバージョンを選ぶ
		result = D3D12CreateDevice(nullptr, i, IID_PPV_ARGS(&dev));
		if (result == S_OK) {
			level = i;
			break;
		}
	}

	//コマンド周り
	ID3D12CommandAllocator*		_commandAllocator = nullptr;//コマンドアロケータ
	ID3D12CommandQueue*			_commandQueue	  = nullptr;//コマンドキュー
	ID3D12GraphicsCommandList*	_commandList	  = nullptr;//コマンドリスト
	//コマンドアロケータの作成
	result = dev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_commandAllocator));
	//コマンドリストの作成
	result = dev->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _commandAllocator, nullptr, IID_PPV_ARGS(&_commandList));
	_commandList->Close();
	//コマンドキューの作成
	D3D12_COMMAND_QUEUE_DESC desc = {};
	desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	desc.NodeMask = 0;
	desc.Priority = 0;
	desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	result = dev->CreateCommandQueue(&desc, IID_PPV_ARGS(&_commandQueue));

	//スワップチェイン周り
	IDXGIFactory4* factory = nullptr;
	result = CreateDXGIFactory1(IID_PPV_ARGS(&factory));
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.Width = WIN_WIDTH;
	swapChainDesc.Height = WIN_HEIGTH;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.Stereo = 0;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	IDXGISwapChain3* swapChain = nullptr;
	result = factory->CreateSwapChainForHwnd(_commandQueue,
		hwnd,
		&swapChainDesc,
		nullptr,
		nullptr,
		(IDXGISwapChain1**)(&swapChain));

	UINT reanderNum = swapChainDesc.BufferCount;

	//ディスクリプタヒープの作成
	D3D12_DESCRIPTOR_HEAP_DESC descriptor = {};
	descriptor.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	descriptor.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	descriptor.NumDescriptors = 2;
	descriptor.NodeMask = 0;
	ID3D12DescriptorHeap* descriptorHeap = nullptr;
	result = dev->CreateDescriptorHeap(&descriptor, IID_PPV_ARGS(&descriptorHeap));
	//ディスクリプタヒープサイズを計算
	UINT heapSize = dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	//ディスクリプタハンドルの作成
	CD3DX12_CPU_DESCRIPTOR_HANDLE descriptorHandle(descriptorHeap->GetCPUDescriptorHandleForHeapStart());

	//レンダーターゲット
	std::vector<ID3D12Resource*> renderTarget;
	//レンダーターゲット数ぶん確保
	renderTarget.resize(reanderNum);
	//ディスクリプタ1個あたりのサイズを取得
	UINT descriptorSize = dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	for(int i = 0; i < reanderNum; ++i){
		result = swapChain->GetBuffer(i, IID_PPV_ARGS(&renderTarget[i]));//スワップチェインからキャンバスを取得
		dev->CreateRenderTargetView(renderTarget[i], nullptr, descriptorHandle);//キャンバスとビューを紐づけ
		descriptorHandle.Offset(descriptorSize);//キャンバスとビューのぶん次のところまでオフセット
	}

	//ルートシグネチャ
	ID3D12RootSignature* rootSignature = nullptr;
	ID3DBlob* signature = nullptr;
	ID3DBlob* error = nullptr;
	D3D12_ROOT_SIGNATURE_DESC rsDesc = {};
	rsDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	result = D3D12SerializeRootSignature(
		&rsDesc,
		D3D_ROOT_SIGNATURE_VERSION_1,
		&signature,
		&error
	);
	result = dev->CreateRootSignature(
		0,
		signature->GetBufferPointer(),
		signature->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature)
	);

	//フェンス
	ID3D12Fence* fence = nullptr;
	UINT64 fenceValue = 0;
	result = dev->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));

	//頂点構造体
	struct VERTEX {
		XMFLOAT3 pos;//座標
	};
	VERTEX vertex;
	//頂点情報の作成
	VERTEX vertices[] = {
		{{0.0f, 0.0f, 0.0f}},
		{{1.0f, 0.0f, 0.0f}},
		{{0.0f, -1.0f, 0.0f}}
	};
	//頂点レイアウトの定義
	D3D12_INPUT_ELEMENT_DESC input[] = {
		{
			"POSITION",									//SemanticName
			0,											//SemanticIndex
			DXGI_FORMAT_R32G32B32_FLOAT,				//Format
			0,											//InputSlot
			D3D12_APPEND_ALIGNED_ELEMENT,				//AlignedByteoffset
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, //InputSlotClass
			0											//InstanceDataStepRate
		},
	};

	//頂点バッファの作成
	ID3D12Resource* vertexBuffer = nullptr;
	result = dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),//CPUからGPUへ転送する用
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(vertices)),//サイズ
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertexBuffer)
	);
	//バッファに対して書き込む
	UCHAR* pData = nullptr;
	result = vertexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&pData));
	memcpy(pData, vertices, sizeof(vertices));//頂点データをバッファにコピー
	vertexBuffer->Unmap(0, nullptr);

	//頂点バッファビューの作成
	D3D12_VERTEX_BUFFER_VIEW vbView = {};
	vbView.BufferLocation			= vertexBuffer->GetGPUVirtualAddress();//頂点アドレスのGPUにあるアドレスを記憶
	vbView.StrideInBytes			= sizeof(vertex);//頂点1つあたりのバイト数を指定
	vbView.SizeInBytes				= sizeof(vertices);//データ全体のサイズを指定

	//シェーダーの読み込み
	ID3DBlob* vertexShader	= nullptr;//頂点シェーダー
	ID3DBlob* pixelShader	= nullptr;//ピクセルシェーダー
	//シェーダーのコンパイルを行う
	result = D3DCompileFromFile(
			_T("Shader.hlsl"), 
			nullptr, 
			nullptr, 
			"BasicVS", 
			"vs_5_0", 
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 
			0, 
			&vertexShader, 
			&error
		);
	result = D3DCompileFromFile(
			_T("Shader.hlsl"), 
			nullptr, 
			nullptr, 
			"BasicPS", 
			"ps_5_0", 
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 
			0, 
			&pixelShader, 
			&error
		);
	
	//パイプラインステートオブジェクト(PSO)
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpsDesc	= {};
	gpsDesc.BlendState							= CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	gpsDesc.DepthStencilState.DepthEnable		= false;
	gpsDesc.DepthStencilState.StencilEnable		= false;
	gpsDesc.VS									= CD3DX12_SHADER_BYTECODE(vertexShader);
	gpsDesc.PS									= CD3DX12_SHADER_BYTECODE(pixelShader);
	gpsDesc.InputLayout.NumElements				= _countof(input);
	gpsDesc.InputLayout.pInputElementDescs		= input;
	gpsDesc.pRootSignature						= rootSignature;
	gpsDesc.RasterizerState						= CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	gpsDesc.RTVFormats[0]						= DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	gpsDesc.PrimitiveTopologyType				= D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	gpsDesc.SampleDesc.Count					= 1;
	gpsDesc.NumRenderTargets					= 1;
	gpsDesc.SampleMask							= UINT_MAX;
	ID3D12PipelineState* piplineState			= nullptr;
	result = dev->CreateGraphicsPipelineState(&gpsDesc, IID_PPV_ARGS(&piplineState));

	//ビューポート
	D3D12_VIEWPORT viewPort = {};
	viewPort.TopLeftX	= 0.0f;
	viewPort.TopLeftY	= 0.0f;
	viewPort.Width		= WIN_WIDTH;
	viewPort.Height		= WIN_HEIGTH;
	viewPort.MinDepth	= 0.0f;
	viewPort.MaxDepth	= 1.0f;

	//ルートシグネチャをセット
	_commandList->SetGraphicsRootSignature(rootSignature);
	//ビューポートをセット
	_commandList->RSSetViewports(1, &viewPort);
	//シザーをセット
	const D3D12_RECT rect = { 0, 0, WIN_WIDTH, WIN_HEIGTH };
	_commandList->RSSetScissorRects(1, &rect);
	

	MSG msg = {};
	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {//OSから投げられてるメッセージをmsgに格納
			TranslateMessage(&msg);	//仮想キー関連の変換
			DispatchMessage(&msg);	//処理されなかったメッセージをOSに返す
		}

		_commandAllocator->Reset();
		_commandList->Reset(_commandAllocator, nullptr);


		_commandList->SetGraphicsRootSignature(rootSignature);

		//リソースバリア
		_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(renderTarget[swapChain->GetCurrentBackBufferIndex()], D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
		
		UINT bbIndex = swapChain->GetCurrentBackBufferIndex();
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(descriptorHeap->GetCPUDescriptorHandleForHeapStart(), bbIndex, descriptorSize);
		_commandList->OMSetRenderTargets(1, &rtvHandle, false, nullptr);
		const FLOAT color[] = { 0.0f, 1.0f, 0.0f, 1.0f };
		_commandList->ClearRenderTargetView(rtvHandle, color, 0, nullptr);
		_commandList->Close();

		ID3D12CommandList* commandList[] = { _commandList };
		_commandQueue->ExecuteCommandLists(_countof(commandList), (ID3D12CommandList* const*)commandList);

		swapChain->Present(1, 0);

		++fenceValue;
		_commandQueue->Signal(fence, fenceValue);
		while (fence->GetCompletedValue() != fenceValue) {
			//待機
		}

		
	}

	//解放
	Release(error);
	Release(signature);
	Release(rootSignature);
	for (UINT i = 0; i < renderTarget.size(); i++){
		Release(renderTarget[i]);
	}
	Release(descriptorHeap);
	Release(swapChain);
	Release(factory);
	Release(_commandList);
	Release(_commandQueue);
	Release(_commandAllocator);
	Release(dev);//オブジェクトの解放
	return 0;
}

//ウィンドウプロシージャ
LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	if (msg == WM_DESTROY) {	//ウィンドウが破棄されたら呼ばれる
		PostQuitMessage(0);	//OSに対してアプリの終了を伝える
		return 0;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);//規定の処理を行う
}
