#include <Windows.h>	//�E�B���h�E�\������̂ɕK�v
#include <d3d12.h>		//DirectX12���g���̂ɕK�v
#include "d3dx12.h"		//DirectX12���g���₷������w�b�_
#include <dxgi1_4.h>	//DXGI�������̂ɕK�v(DX12�ł�DXGI1.4���g���Ă�)
#include <D3Dcompiler.h>//�V�F�[�_�R���p�C���ŕK�v
#include <DirectXMath.h>//���w�n�֗̕��Ȃ̂������Ă�w�b�_
#include <tchar.h>
#include <vector>

using namespace DirectX;

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#define WIN_WIDTH	(640)	//�E�B���h�E�T�C�Y��
#define WIN_HEIGTH	(480)	//�E�B���h�E�T�C�Y��

#define Release(X)	{if((X) != nullptr) (X)->Release();}

//�v���g�^�C�v�錾
LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	WNDCLASSEX w = {};
	w.cbSize = sizeof(WNDCLASSEX);
	w.lpfnWndProc = (WNDPROC)WindowProcedure;//�R�[���o�b�N�֐��̎w��
	w.lpszClassName = _T("DirectX12");//�A�v���P�[�V�����N���X��
	w.hInstance = GetModuleHandle(0);//�n���h���̎擾
	RegisterClassEx(&w);//�A�v���P�[�V�����N���X

	//�E�B���h�E�T�C�Y�ݒ�
	RECT wrc = { 0, 0, WIN_WIDTH, WIN_HEIGTH };//�E�B���h�E�T�C�Y��ݒ�
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);//�␳

	//�E�B���h�E����
	HWND hwnd = CreateWindow(w.lpszClassName,//�N���X���w��
		_T("DirectX12"),			//�^�C�g���o�[�̕���
		WS_OVERLAPPEDWINDOW,		//�^�C�g���o�[�Ƌ��E��������E�B���h�E
		CW_USEDEFAULT,			//�\��X���W��OS�ɂ��C��
		CW_USEDEFAULT,			//�\��Y���W��OS�ɂ��C��
		wrc.right - wrc.left,		//�E�B���h�E��
		wrc.bottom - wrc.top,		//�E�B���h�E��
		nullptr,					//�e�E�B���h�E�̃n���h��
		nullptr,					//���j���[�n���h��
		w.hInstance,				//�Ăяo���A�v���P�[�V�����n���h��
		nullptr);				//�ǉ��p�����[�^

	//�E�B���h�E�\��
	ShowWindow(hwnd, SW_SHOW);

	//������
	D3D_FEATURE_LEVEL levels[] = {
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0
	};
	D3D_FEATURE_LEVEL level = {};
	ID3D12Device* dev = nullptr;
	HRESULT result = S_OK;
	//�G���[���o�͂ɕ\��������
#ifdef _DEBUG
	ID3D12Debug *debug = nullptr;
	result = D3D12GetDebugInterface(IID_PPV_ARGS(&debug));
	if (FAILED(result))
		int i = 0;
	debug->EnableDebugLayer();
	Release(debug);
	debug = nullptr;
#endif
	for (auto i : levels) {	//�}�V���ɂ������o�[�W������I��
		result = D3D12CreateDevice(nullptr, i, IID_PPV_ARGS(&dev));
		if (result == S_OK) {
			level = i;
			break;
		}
	}

	//�R�}���h����
	ID3D12CommandAllocator*		_commandAllocator = nullptr;//�R�}���h�A���P�[�^
	ID3D12CommandQueue*			_commandQueue	  = nullptr;//�R�}���h�L���[
	ID3D12GraphicsCommandList*	_commandList	  = nullptr;//�R�}���h���X�g
	//�R�}���h�A���P�[�^�̍쐬
	result = dev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_commandAllocator));
	//�R�}���h���X�g�̍쐬
	result = dev->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _commandAllocator, nullptr, IID_PPV_ARGS(&_commandList));
	_commandList->Close();
	//�R�}���h�L���[�̍쐬
	D3D12_COMMAND_QUEUE_DESC desc = {};
	desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	desc.NodeMask = 0;
	desc.Priority = 0;
	desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	result = dev->CreateCommandQueue(&desc, IID_PPV_ARGS(&_commandQueue));

	//�X���b�v�`�F�C������
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

	//�f�B�X�N���v�^�q�[�v�̍쐬
	D3D12_DESCRIPTOR_HEAP_DESC descriptor = {};
	descriptor.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	descriptor.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	descriptor.NumDescriptors = 2;
	descriptor.NodeMask = 0;
	ID3D12DescriptorHeap* descriptorHeap = nullptr;
	result = dev->CreateDescriptorHeap(&descriptor, IID_PPV_ARGS(&descriptorHeap));
	//�f�B�X�N���v�^�q�[�v�T�C�Y���v�Z
	UINT heapSize = dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	//�f�B�X�N���v�^�n���h���̍쐬
	CD3DX12_CPU_DESCRIPTOR_HANDLE descriptorHandle(descriptorHeap->GetCPUDescriptorHandleForHeapStart());

	//�����_�[�^�[�Q�b�g
	std::vector<ID3D12Resource*> renderTarget;
	//�����_�[�^�[�Q�b�g���Ԃ�m��
	renderTarget.resize(reanderNum);
	//�f�B�X�N���v�^1������̃T�C�Y���擾
	UINT descriptorSize = dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	for(int i = 0; i < reanderNum; ++i){
		result = swapChain->GetBuffer(i, IID_PPV_ARGS(&renderTarget[i]));//�X���b�v�`�F�C������L�����o�X���擾
		dev->CreateRenderTargetView(renderTarget[i], nullptr, descriptorHandle);//�L�����o�X�ƃr���[��R�Â�
		descriptorHandle.Offset(descriptorSize);//�L�����o�X�ƃr���[�̂Ԃ񎟂̂Ƃ���܂ŃI�t�Z�b�g
	}

	//���[�g�V�O�l�`��
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

	//�t�F���X
	ID3D12Fence* fence = nullptr;
	UINT64 fenceValue = 0;
	result = dev->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));

	//���_�\����
	struct VERTEX {
		XMFLOAT3 pos;//���W
		XMFLOAT2 uv;//uv���W
	};
	VERTEX vertex;
	//���_���̍쐬
	VERTEX vertices[] = {
		{{-1.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
		{{1.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
		{{-1.0f, -1.0f, 0.0f}, {0.0f, 1.0f}},

		{{1.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
		{{1.0f, -1.0f, 0.0f}, {1.0f, 1.0f}},
		{{-1.0f, -1.0f, 0.0f}, {0.0f, 1.0f}}
	};
	//���_���C�A�E�g�̒�`
	D3D12_INPUT_ELEMENT_DESC input[] = {
		//���_
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

	//���_�o�b�t�@�̍쐬
	ID3D12Resource* vertexBuffer = nullptr;
	result = dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),//CPU����GPU�֓]������p
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(vertices)),//�T�C�Y
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertexBuffer)
	);
	//�o�b�t�@�ɑ΂��ď�������
	UCHAR* pData = nullptr;
	result = vertexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&pData));
	memcpy(pData, vertices, sizeof(vertices));//���_�f�[�^���o�b�t�@�ɃR�s�[
	vertexBuffer->Unmap(0, nullptr);

	//���_�o�b�t�@�r���[�̍쐬
	D3D12_VERTEX_BUFFER_VIEW vbView = {};
	vbView.BufferLocation			= vertexBuffer->GetGPUVirtualAddress();//���_�A�h���X��GPU�ɂ���A�h���X���L��
	vbView.StrideInBytes			= sizeof(VERTEX);//���_1������̃o�C�g�����w��
	vbView.SizeInBytes				= sizeof(vertices);//�f�[�^�S�̂̃T�C�Y���w��


	//�e�N�X�`�����\�[�X�̍쐬
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

	//�e�N�X�`���̓ǂݍ���
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
	data.resize(bmpInfoHeader.biSizeImage);//�̈�m��
	//bmp�f�[�^�����ׂēǂݍ���
	for(UINT i = 0; i < data.size(); ++i)
	{
		fread(&data[i], sizeof(UCHAR), 1, fp);
	}
	fclose(fp);

	//�e�N�X�`���o�b�t�@�ւ̏�������
	D3D12_BOX box = {};
	box.left = 0;
	box.top = 0;
	box.front = 0;
	box.right = 256;
	box.bottom = 256;
	box.back = 1;
	ID3D12Resource* resource = nullptr;
	//result = resource->WriteToSubresource(
	//	0,
	//	&box,
	//	&data,
	//	,
	//	data.size()
	//);


	//�V�F�[�_�[�̓ǂݍ���
	ID3DBlob* vertexShader	= nullptr;//���_�V�F�[�_�[
	ID3DBlob* pixelShader	= nullptr;//�s�N�Z���V�F�[�_�[
	//�V�F�[�_�[�̃R���p�C�����s��
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
	
	//�p�C�v���C���X�e�[�g�I�u�W�F�N�g(PSO)
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

	//�r���[�|�[�g
	D3D12_VIEWPORT viewPort = {};
	viewPort.TopLeftX		= 0.0f;
	viewPort.TopLeftY		= 0.0f;
	viewPort.Width			= WIN_WIDTH;
	viewPort.Height			= WIN_HEIGTH;
	viewPort.MinDepth		= 0.0f;
	viewPort.MaxDepth		= 1.0f;

	

	MSG msg = {};
	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {//OS���瓊�����Ă郁�b�Z�[�W��msg�Ɋi�[
			TranslateMessage(&msg);	//���z�L�[�֘A�̕ϊ�
			DispatchMessage(&msg);	//��������Ȃ��������b�Z�[�W��OS�ɕԂ�
		}

		_commandAllocator->Reset();
		_commandList->Reset(_commandAllocator, nullptr);

		//���[�g�V�O�l�`���̃Z�b�g
		_commandList->SetGraphicsRootSignature(rootSignature);

		//�p�C�v���C���̃Z�b�g
		_commandList->SetPipelineState(piplineState);

		//�r���[�|�[�g���Z�b�g
		_commandList->RSSetViewports(1, &viewPort);
		//�V�U�[���Z�b�g
		const D3D12_RECT rect = { 0, 0, WIN_WIDTH, WIN_HEIGTH };
		_commandList->RSSetScissorRects(1, &rect);

		//���_�o�b�t�@�̃Z�b�g
		_commandList->IASetVertexBuffers(0, 1, &vbView);
		
		UINT bbIndex = swapChain->GetCurrentBackBufferIndex();
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(descriptorHeap->GetCPUDescriptorHandleForHeapStart(), bbIndex, descriptorSize);
		_commandList->OMSetRenderTargets(1, &rtvHandle, false, nullptr);
		const FLOAT color[] = { 0.0f, 1.0f, 0.0f, 1.0f };
		_commandList->ClearRenderTargetView(rtvHandle, color, 0, nullptr);

		//���\�[�X�o���A
		_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(renderTarget[swapChain->GetCurrentBackBufferIndex()], D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
		
		//�O�p�|���S���`��ɂ���
		_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		
		//���_�`��
		_commandList->DrawInstanced(6, 1, 0, 0);

		_commandList->Close();

		ID3D12CommandList* commandList[] = { _commandList };
		_commandQueue->ExecuteCommandLists(_countof(commandList), (ID3D12CommandList* const*)commandList);

		swapChain->Present(1, 0);

		++fenceValue;
		_commandQueue->Signal(fence, fenceValue);
		while (fence->GetCompletedValue() != fenceValue) {
			//�ҋ@
		}

		
	}

	//���
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
	Release(dev);//�I�u�W�F�N�g�̉��
	return 0;
}

//�E�B���h�E�v���V�[�W��
LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	if (msg == WM_DESTROY) {	//�E�B���h�E���j�����ꂽ��Ă΂��
		PostQuitMessage(0);	//OS�ɑ΂��ăA�v���̏I����`����
		return 0;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);//�K��̏������s��
}
