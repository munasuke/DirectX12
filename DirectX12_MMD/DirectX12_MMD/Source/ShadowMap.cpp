#include "ShadowMap.h"
#include "Window.h"
#include <d3dcompiler.h>


ShadowMap::ShadowMap(ID3D12Device* dev, unsigned int inputDescNum, D3D12_INPUT_ELEMENT_DESC* inputDesc) : resource(nullptr), gps(nullptr), rs(nullptr) {
	heap["DSV"] = nullptr;
	heap["SRV"] = nullptr;

	auto size = Size(max(WIN_HEIGHT, WIN_WIDTH));

	//バッファ生成
	D3D12_RESOURCE_DESC resDesc = {};
	resDesc.Alignment			= 0;
	resDesc.DepthOrArraySize	= 1;
	resDesc.Dimension			= D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resDesc.Flags				= D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	resDesc.Format				= DXGI_FORMAT::DXGI_FORMAT_R32_TYPELESS;
	resDesc.Height				= size;
	resDesc.Width				= size;
	resDesc.Layout				= D3D12_TEXTURE_LAYOUT::D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	resDesc.MipLevels			= 1;
	resDesc.SampleDesc.Count	= 1;
	resDesc.SampleDesc.Quality	= 0;

	D3D12_HEAP_PROPERTIES prop = {};
	prop.CPUPageProperty		= D3D12_CPU_PAGE_PROPERTY::D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	prop.MemoryPoolPreference	= D3D12_MEMORY_POOL::D3D12_MEMORY_POOL_UNKNOWN;
	prop.Type					= D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_DEFAULT;

	D3D12_CLEAR_VALUE cv = {};
	cv.DepthStencil.Depth	= 1.0f;
	cv.Format				= DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT;

	auto result = dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&cv,
		IID_PPV_ARGS(&resource)
	);

	//DSVヒープの生成
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.Flags			= D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	heapDesc.NodeMask		= 0;
	heapDesc.NumDescriptors = 1;
	heapDesc.Type			= D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_DSV;

	result = dev->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&heap["DSV"]));

	//DSV作成
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format				= DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension		= D3D12_DSV_DIMENSION::D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Flags				= D3D12_DSV_FLAGS::D3D12_DSV_FLAG_NONE;
	dsvDesc.Texture2D.MipSlice	= 0;

	dev->CreateDepthStencilView(resource, &dsvDesc, heap["DSV"]->GetCPUDescriptorHandleForHeapStart());

	//SRVヒープ生成
	heapDesc.Type	= D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	heapDesc.Flags	= D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	result = dev->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&heap["SRV"]));

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format					= DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension			= D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels		= 1;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	dev->CreateShaderResourceView(resource, &srvDesc, heap["SRV"]->GetCPUDescriptorHandleForHeapStart());

	//デスクリプタレンジ
	D3D12_DESCRIPTOR_RANGE dr[1] = {};
	dr[0].BaseShaderRegister				= 0;
	dr[0].NumDescriptors					= 1;
	dr[0].RangeType							= D3D12_DESCRIPTOR_RANGE_TYPE::D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	dr[0].RegisterSpace						= 0;
	dr[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	//ルートパラメータ
	D3D12_ROOT_PARAMETER rp[1] = {};
	rp[0].ParameterType							= D3D12_ROOT_PARAMETER_TYPE::D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rp[0].DescriptorTable.NumDescriptorRanges	= 1;
	rp[0].DescriptorTable.pDescriptorRanges		= &dr[0];
	rp[0].ShaderVisibility						= D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_ALL;

	//ルートシグネチャ
	D3D12_ROOT_SIGNATURE_DESC rsDesc = {};
	rsDesc.NumParameters		= _countof(rp);
	rsDesc.pParameters			= rp;
	rsDesc.pStaticSamplers		= nullptr;
	rsDesc.NumStaticSamplers	= 1;
	rsDesc.Flags				= D3D12_ROOT_SIGNATURE_FLAGS::D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	ID3DBlob* signature = nullptr;
	ID3DBlob* error		= nullptr;

	result = D3D12SerializeRootSignature(
		&rsDesc,
		D3D_ROOT_SIGNATURE_VERSION::D3D_ROOT_SIGNATURE_VERSION_1,
		&signature,
		&error
	);

	result = dev->CreateRootSignature(
		0,
		signature->GetBufferPointer(),
		signature->GetBufferSize(),
		IID_PPV_ARGS(&rs)
	);

	signature->Release();

	ID3DBlob* vs = nullptr;
	ID3DBlob* ps = nullptr;

	result = D3DCompileFromFile(L"Light.hlsl", nullptr, nullptr, "VS", "vs_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &vs, &error);
	result = D3DCompileFromFile(L"Light.hlsl", nullptr, nullptr, "PS", "ps_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &ps, &error);

	//パイプライン
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpsDesc = {};
	gpsDesc.BlendState							= CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	gpsDesc.DepthStencilState.DepthEnable		= true;													//深度バッファを有効にする
	gpsDesc.DepthStencilState.DepthWriteMask	= D3D12_DEPTH_WRITE_MASK::D3D12_DEPTH_WRITE_MASK_ALL;	//常に深度値を書き込む
	gpsDesc.DepthStencilState.DepthFunc			= D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_LESS;	//深度値の結果が小さい方(カメラに近い方)を書き込む
	gpsDesc.DSVFormat							= DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT;					//深度バッファのフォーマットを明示する
	gpsDesc.DepthStencilState.StencilEnable		= false;
	gpsDesc.VS									= CD3DX12_SHADER_BYTECODE(vs);
	gpsDesc.PS									= CD3DX12_SHADER_BYTECODE(ps);
	gpsDesc.InputLayout.NumElements				= inputDescNum;
	gpsDesc.InputLayout.pInputElementDescs		= inputDesc;
	gpsDesc.pRootSignature						= rs;
	gpsDesc.RasterizerState						= CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	gpsDesc.RasterizerState.CullMode			= D3D12_CULL_MODE::D3D12_CULL_MODE_NONE;
	gpsDesc.RTVFormats[0]						= DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
	gpsDesc.PrimitiveTopologyType				= D3D12_PRIMITIVE_TOPOLOGY_TYPE::D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	gpsDesc.SampleDesc.Count					= 1;
	gpsDesc.NumRenderTargets					= 1;
	gpsDesc.SampleMask							= UINT_MAX;

	result = dev->CreateGraphicsPipelineState(&gpsDesc, IID_PPV_ARGS(&gps));
}

void ShadowMap::Draw(ID3D12GraphicsCommandList * list, const unsigned int materialNum) {
	list->SetPipelineState(gps);
	list->SetGraphicsRootSignature(rs);
	
	list->DrawIndexedInstanced(materialNum, 1, 0, 0, 0);
}


ShadowMap::~ShadowMap() {
}

size_t ShadowMap::Size(size_t size) {
	size_t bit = 0x80000000;

	//1ずつビットを下ろしていく
	for (size_t i = 31; i >= 0; --i) {
		//立っていた場合は抜ける
		if (size & bit) {
			break;
		}
		bit >>= 1;
	}

	return bit + (bit % size);
}
