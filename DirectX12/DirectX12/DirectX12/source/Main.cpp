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
	WNDCLASSEX w	= {};
	w.cbSize		= sizeof(WNDCLASSEX);
	w.lpfnWndProc	= (WNDPROC)WindowProcedure;//コールバック関数の指定
	w.lpszClassName = _T("DirectX12");//アプリケーションクラス名
	w.hInstance		= GetModuleHandle(0);//ハンドルの取得
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
	ID3D12Device* dev		= nullptr;
	HRESULT result			= S_OK;
	//エラーを出力に表示させる
#ifdef _DEBUG
	ID3D12Debug *debug	= nullptr;
	result				= D3D12GetDebugInterface(IID_PPV_ARGS(&debug));
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
	D3D12_COMMAND_QUEUE_DESC desc	= {};
	desc.Flags						= D3D12_COMMAND_QUEUE_FLAG_NONE;
	desc.NodeMask					= 0;
	desc.Priority					= 0;
	desc.Type						= D3D12_COMMAND_LIST_TYPE_DIRECT;
	result = dev->CreateCommandQueue(&desc, IID_PPV_ARGS(&_commandQueue));

	//スワップチェイン周り
	IDXGIFactory4* factory = nullptr;
	result = CreateDXGIFactory1(IID_PPV_ARGS(&factory));
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.Width					= WIN_WIDTH;
	swapChainDesc.Height				= WIN_HEIGTH;
	swapChainDesc.Format				= DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.Stereo				= 0;
	swapChainDesc.SampleDesc.Count		= 1;
	swapChainDesc.SampleDesc.Quality	= 0;
	swapChainDesc.BufferUsage			= DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount			= 2;
	swapChainDesc.Scaling				= DXGI_SCALING_STRETCH;
	swapChainDesc.SwapEffect			= DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.AlphaMode				= DXGI_ALPHA_MODE_UNSPECIFIED;
	IDXGISwapChain3* swapChain = nullptr;
	result = factory->CreateSwapChainForHwnd(_commandQueue,
		hwnd,
		&swapChainDesc,
		nullptr,
		nullptr,
		(IDXGISwapChain1**)(&swapChain));

	UINT reanderNum = swapChainDesc.BufferCount;

	//ディスクリプタヒープの作成
	D3D12_DESCRIPTOR_HEAP_DESC descriptor	= {};
	descriptor.Type							= D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	descriptor.Flags						= D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	descriptor.NumDescriptors				= 2;
	descriptor.NodeMask						= 0;
	ID3D12DescriptorHeap* descriptorHeap = nullptr;
	result = dev->CreateDescriptorHeap(&descriptor, IID_PPV_ARGS(&descriptorHeap));
	//ディスクリプタハンドルの作成
	CD3DX12_CPU_DESCRIPTOR_HANDLE descriptorHandle(descriptorHeap->GetCPUDescriptorHandleForHeapStart());

	//レンダーターゲット
	std::vector<ID3D12Resource*> renderTarget;
	//レンダーターゲット数ぶん確保
	renderTarget.resize(reanderNum);
	//ディスクリプタ1個あたりのサイズを取得
	UINT descriptorSize = dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	for(UINT i = 0; i < reanderNum; ++i){
		result = swapChain->GetBuffer(i, IID_PPV_ARGS(&renderTarget[i]));//スワップチェインからキャンバスを取得
		dev->CreateRenderTargetView(renderTarget[i], nullptr, descriptorHandle);//キャンバスとビューを紐づけ
		descriptorHandle.Offset(descriptorSize);//キャンバスとビューのぶん次のところまでオフセット
	}

	//サンプラの設定
	D3D12_STATIC_SAMPLER_DESC samplerDesc	= {};
	samplerDesc.Filter						= D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT;
	samplerDesc.AddressU					= D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.AddressV					= D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.AddressW					= D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.MaxLOD						= D3D12_FLOAT32_MAX;
	samplerDesc.MinLOD						= 0.0f;
	samplerDesc.MipLODBias					= 0.0f;
	samplerDesc.BorderColor					= D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	samplerDesc.ShaderRegister				= 0;
	samplerDesc.ShaderVisibility			= D3D12_SHADER_VISIBILITY_PIXEL;
	samplerDesc.RegisterSpace				= 0;
	samplerDesc.MaxAnisotropy				= 0;
	samplerDesc.ComparisonFunc				= D3D12_COMPARISON_FUNC_NEVER;

	//ディスクリプタレンジの設定
	D3D12_DESCRIPTOR_RANGE dRange[1];
	SecureZeroMemory(&dRange, sizeof(dRange));

	//ルートパラメータの設定
	D3D12_ROOT_PARAMETER parameter[1];
	SecureZeroMemory(&parameter, sizeof(parameter));

	dRange[0].RangeType							= D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	dRange[0].NumDescriptors					= 1;
	dRange[0].BaseShaderRegister				= 0;
	dRange[0].RegisterSpace						= 0;
	dRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	parameter[0].ParameterType							= D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	parameter[0].ShaderVisibility						= D3D12_SHADER_VISIBILITY_ALL;
	parameter[0].DescriptorTable.NumDescriptorRanges	= 1;
	parameter[0].DescriptorTable.pDescriptorRanges		= &dRange[0];

	//ルートシグネチャ
	ID3D12RootSignature* rootSignature	= nullptr;
	ID3DBlob* signature					= nullptr;
	ID3DBlob* error						= nullptr;
	D3D12_ROOT_SIGNATURE_DESC rsDesc	= {};
	rsDesc.NumParameters				= _countof(parameter);
	rsDesc.NumStaticSamplers			= 1;
	rsDesc.pParameters					= parameter;
	rsDesc.pStaticSamplers				= &samplerDesc;
	rsDesc.Flags						= D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
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
	ID3D12Fence* fence	= nullptr;
	UINT64 fenceValue	= 0;
	result = dev->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));

	//頂点構造体
	struct VERTEX {
		XMFLOAT3 pos;//座標
		XMFLOAT2 uv;//uv座標
	};
	
	//頂点情報の作成
	VERTEX vertices[] = {
		{{-0.7f,  0.7f, 0.0f}, {0.0f, 0.0f}},
		{{ 0.7f,  0.7f, 0.0f}, {1.0f, 0.0f}},
		{{ 0.7f, -0.7f, 0.0f}, {1.0f, 1.0f}},

		{{ 0.7f, -0.7f, 0.0f}, {1.0f, 1.0f}},
		{{-0.7f, -0.7f, 0.0f}, {0.0f, 1.0f}},
		{{-0.7f,  0.7f, 0.0f}, {0.0f, 0.0f}}
	};
	//頂点レイアウトの定義
	D3D12_INPUT_ELEMENT_DESC input[] = {
		//頂点
		{
			"POSITION",									//SemanticName
			0,											//SemanticIndex
			DXGI_FORMAT_R32G32B32_FLOAT,				//Format
			0,											//InputSlot
			D3D12_APPEND_ALIGNED_ELEMENT,				//AlignedByteoffset
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, //InputSlotClass
			0											//InstanceDataStepRate
		},
		//uv
		{
			"TEXCOORD",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		}
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
	vbView.StrideInBytes			= sizeof(VERTEX);//頂点1つあたりのバイト数を指定
	vbView.SizeInBytes				= sizeof(vertices);//データ全体のサイズを指定


	//テクスチャリソースの作成
	CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	D3D12_RESOURCE_DESC texResourceDesc = {};
	texResourceDesc.Dimension			= D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	texResourceDesc.Width				= 256;
	texResourceDesc.Height				= 256;
	texResourceDesc.DepthOrArraySize	= 1;
	texResourceDesc.Format				= DXGI_FORMAT_R8G8B8A8_UNORM;
	texResourceDesc.SampleDesc.Count	= 1;
	texResourceDesc.Flags				= D3D12_RESOURCE_FLAG_NONE;
	texResourceDesc.Layout				= D3D12_TEXTURE_LAYOUT_UNKNOWN;

	D3D12_HEAP_PROPERTIES hprop = {};
	hprop.Type					= D3D12_HEAP_TYPE_CUSTOM;
	hprop.CPUPageProperty		= D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	hprop.MemoryPoolPreference	= D3D12_MEMORY_POOL_L0;
	hprop.CreationNodeMask		= 1;
	hprop.VisibleNodeMask		= 1;

	ID3D12Resource* textureBuffer = nullptr;
	result = dev->CreateCommittedResource(
		&hprop,
		D3D12_HEAP_FLAG_NONE,
		&texResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&textureBuffer)
	);

	//シェーダリソースビューの作成
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.NumDescriptors				= 1;
	heapDesc.Type						= D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	heapDesc.Flags						= D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	ID3D12DescriptorHeap* texHeap = nullptr;
	result = dev->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&texHeap));

	UINT stride = dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	D3D12_SHADER_RESOURCE_VIEW_DESC sDesc	= {};
	sDesc.Format							= DXGI_FORMAT_R8G8B8A8_UNORM;
	sDesc.ViewDimension						= D3D12_SRV_DIMENSION_TEXTURE2D;
	sDesc.Texture2D.MipLevels				= 1;
	sDesc.Shader4ComponentMapping			= D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	dev->CreateShaderResourceView(textureBuffer, &sDesc, texHeap->GetCPUDescriptorHandleForHeapStart());

	//テクスチャの読み込み
	BITMAPFILEHEADER bmpFileHeader = {};
	BITMAPINFOHEADER bmpInfoHeader = {};
	FILE* fp = nullptr;
	if (fopen_s(&fp, "image/aoba.bmp", "rb") != 0){
		return -1;
	}
	//bmpFileHeader
	fread(&bmpFileHeader, sizeof(bmpFileHeader), 1, fp);
	//bmpInfoHeader
	fread(&bmpInfoHeader, sizeof(bmpInfoHeader), 1, fp);

	DWORD rgb[3];
	fread(&rgb[0], sizeof(rgb), 1, fp);

	std::vector<CHAR> data;
	data.resize(bmpInfoHeader.biSizeImage);//領域確保
	//bmpデータをすべて読み込む
	for(INT line = bmpInfoHeader.biHeight - 1; line >= 0; --line) {//下から1ラインずつ上がる
		for (INT count = 0; count < bmpInfoHeader.biWidth * 4; count += 4){//左から右へ
			UINT address = line * bmpInfoHeader.biWidth * 4;
			fread(&data[address + count], sizeof(UCHAR), 4, fp);
		}
	}
	fclose(fp);


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
	gpsDesc.RTVFormats[0]						= DXGI_FORMAT_R8G8B8A8_UNORM;
	gpsDesc.PrimitiveTopologyType				= D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	gpsDesc.SampleDesc.Count					= 1;
	gpsDesc.NumRenderTargets					= 1;
	gpsDesc.SampleMask							= UINT_MAX;
	ID3D12PipelineState* piplineState			= nullptr;
	result = dev->CreateGraphicsPipelineState(&gpsDesc, IID_PPV_ARGS(&piplineState));

	//ビューポート
	D3D12_VIEWPORT viewPort = {};
	viewPort.TopLeftX		= 0.0f;
	viewPort.TopLeftY		= 0.0f;
	viewPort.Width			= WIN_WIDTH;
	viewPort.Height			= WIN_HEIGTH;
	viewPort.MinDepth		= 0.0f;
	viewPort.MaxDepth		= 1.0f;


	//ワールド行列
	XMMATRIX world = XMMatrixIdentity();//単位行列で初期化

	XMVECTOR eye = { 0.0f, 0.0f, -10.0f };//視点
	XMVECTOR target = { 0.0f, 0.0f, 0.0f };//注視点
	XMVECTOR upper = { 0.0f, 1.0f, 0.0f };//上ベクトル

	//ビュー行列
	XMMATRIX camera = XMMatrixLookAtLH(eye, target, upper);
	//プロジェクション行列
	XMMATRIX projection = XMMatrixPerspectiveFovLH(0.0f, static_cast<float>(WIN_HEIGTH / WIN_WIDTH), 0.01f, 1000.0f);

	//行列の合成
	XMMATRIX matrix = world * camera * projection;


	//ループ
	MSG msg = {};
	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {//OSから投げられてるメッセージをmsgに格納
			TranslateMessage(&msg);	//仮想キー関連の変換
			DispatchMessage(&msg);	//処理されなかったメッセージをOSに返す
		}

		_commandAllocator->Reset();
		_commandList->Reset(_commandAllocator, piplineState);

		//ルートシグネチャのセット
		_commandList->SetGraphicsRootSignature(rootSignature);

		//パイプラインのセット
		_commandList->SetPipelineState(piplineState);

		//ビューポートをセット
		_commandList->RSSetViewports(1, &viewPort);
		//シザーをセット
		const D3D12_RECT rect = { 0, 0, WIN_WIDTH, WIN_HEIGTH };
		_commandList->RSSetScissorRects(1, &rect);

		//シェーダリソースビュー用のデスクリプタをセット
		_commandList->SetDescriptorHeaps(1, (ID3D12DescriptorHeap* const*)&texHeap);
		_commandList->SetGraphicsRootDescriptorTable(0, texHeap->GetGPUDescriptorHandleForHeapStart());

		_commandList->ResourceBarrier(
			1,
			&CD3DX12_RESOURCE_BARRIER::Transition(renderTarget[swapChain->GetCurrentBackBufferIndex()], D3D12_RESOURCE_STATE_PRESENT,  D3D12_RESOURCE_STATE_RENDER_TARGET)
		);


		//頂点バッファのセット
		_commandList->IASetVertexBuffers(0, 1, &vbView);
		
		UINT bbIndex = swapChain->GetCurrentBackBufferIndex();
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(descriptorHeap->GetCPUDescriptorHandleForHeapStart(), bbIndex, descriptorSize);
		_commandList->OMSetRenderTargets(1, &rtvHandle, false, nullptr);
		const FLOAT color[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		_commandList->ClearRenderTargetView(rtvHandle, color, 0, nullptr);
		
		//三角ポリゴン描画にする
		_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//テクスチャバッファへの書き込み
		D3D12_BOX box	= {};
		box.left		= 0;
		box.top			= 0;
		box.front		= 0;
		box.right		= 256;
		box.bottom		= 256;
		box.back		= 1;
		result = textureBuffer->WriteToSubresource(
			0,
			&box,
			data.data(),
			box.right * 4,
			box.bottom * 4
		);

		//頂点描画
		_commandList->DrawInstanced(6, 1, 0, 0);

		//リソースバリア
		_commandList->ResourceBarrier(
			1,
			&CD3DX12_RESOURCE_BARRIER::Transition(textureBuffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE)
		);

		_commandList->Close();

		ID3D12CommandList* commandList[] = { _commandList };
		_commandQueue->ExecuteCommandLists(_countof(commandList), commandList);



		result = swapChain->Present(1, 0);
		if (FAILED(result)) 
		{

			result = dev->GetDeviceRemovedReason();
		}

		_commandQueue->Signal(fence, ++fenceValue);
		while (fence->GetCompletedValue() != fenceValue) {
			//待機
		}
	}

	//解放
	Release(piplineState);
	Release(fence);
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
