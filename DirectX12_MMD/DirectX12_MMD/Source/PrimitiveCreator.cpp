#include "PrimitiveCreator.h"
#include "PrimitiveObject.h"
#include "Plane.h"
#include "Camera.h"
#include "Window.h"
#include <d3dcompiler.h>

PrimitiveManager::PrimitiveManager(ID3D12Device* d) : piplineState(nullptr), rs(nullptr), resource(nullptr), dev(nullptr){
	dev = d;

	//サンプラ
	D3D12_STATIC_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter				= D3D12_FILTER::D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU			= D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.AddressV			= D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.AddressW			= D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.MaxLOD				= D3D12_FLOAT32_MAX;
	samplerDesc.MinLOD				= 0.0f;
	samplerDesc.MipLODBias			= 0.0f;
	samplerDesc.BorderColor			= D3D12_STATIC_BORDER_COLOR::D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	samplerDesc.ShaderRegister		= 0;
	samplerDesc.ShaderVisibility	= D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_ALL;
	samplerDesc.RegisterSpace		= 0;
	samplerDesc.MaxAnisotropy		= 0;
	samplerDesc.ComparisonFunc		= D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_NEVER;

	//デスクリプタレンジ
	//D3D12_DESCRIPTOR_RANGE dr[1] = {};
	//dr[0].BaseShaderRegister				= 0;
	//dr[0].NumDescriptors					= 1;
	//dr[0].RangeType							= D3D12_DESCRIPTOR_RANGE_TYPE::D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	//dr[0].RegisterSpace						= 0;
	//dr[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	//ルートパラメータ
	//カメラ(b0)
	D3D12_ROOT_PARAMETER rp[1] = {};
	rp[0].ParameterType				= D3D12_ROOT_PARAMETER_TYPE::D3D12_ROOT_PARAMETER_TYPE_CBV;
	rp[0].ShaderVisibility			= D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_ALL;
	rp[0].Descriptor.RegisterSpace	= 0;
	rp[0].Descriptor.ShaderRegister = 0;

	//rp[0].ParameterType							= D3D12_ROOT_PARAMETER_TYPE::D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	//rp[0].DescriptorTable.NumDescriptorRanges	= 1;
	//rp[0].DescriptorTable.pDescriptorRanges		= &dr[0];
	//rp[0].ShaderVisibility						= D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_ALL;

	//ルートシグネチャ
	D3D12_ROOT_SIGNATURE_DESC rsDesc = {};
	rsDesc.NumParameters		= _countof(rp);
	rsDesc.pParameters			= rp;
	rsDesc.pStaticSamplers		= &samplerDesc;
	rsDesc.NumStaticSamplers	= 1;
	rsDesc.Flags				= D3D12_ROOT_SIGNATURE_FLAGS::D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	ID3DBlob* signature = nullptr;
	ID3DBlob* error		= nullptr;

	auto result = D3D12SerializeRootSignature(
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

	//視線、注視点、上ベクトル
	XMFLOAT3 eye	(0.0f, 14.0f, -13.0f);
	XMFLOAT3 focus	(0.0f, 12.0f,   0.0f);
	XMFLOAT3 upper	(0.0f,  1.0f,   0.0f);
	auto target = XMFLOAT4(0.0f, 12.0f, 0.0f, 1.0f);
	//auto distance = ;//視線ベクトルのサイズ
	//ワールド・ビュー・プロジェクション行列の作成
	Matrix mt;
	mt.world		= XMMatrixIdentity();
	mt.view			= XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&focus), XMLoadFloat3(&upper));
	mt.projection	= XMMatrixPerspectiveFovLH(XM_PIDIV2/*90.0f * 3.14159264f / 180.0f*/,
		static_cast<FLOAT>(WIN_WIDTH) / static_cast<FLOAT>(WIN_HEIGHT), 0.01f, 500.0f);

	//プロパティ設定
	CD3DX12_HEAP_PROPERTIES heapProperties = {};
	heapProperties.Type					= D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD;
	heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL::D3D12_MEMORY_POOL_UNKNOWN;
	heapProperties.CPUPageProperty		= D3D12_CPU_PAGE_PROPERTY::D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProperties.VisibleNodeMask		= 1;
	heapProperties.CreationNodeMask		= 1;

	//リソース設定
	D3D12_RESOURCE_DESC cbvResDesc = {};
	cbvResDesc.Dimension		= D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_BUFFER;//1次元バッファ
	cbvResDesc.Width			= (sizeof(mt) + 0xff) &~0xff;								//255アライメント
	cbvResDesc.Height			= 1;														//1次元なので１を設定
	cbvResDesc.DepthOrArraySize = 1;														//深さはないので１を設定
	cbvResDesc.MipLevels		= 1;														//ミップはない
	cbvResDesc.SampleDesc.Count = 1;														//ないと失敗する
	cbvResDesc.Layout			= D3D12_TEXTURE_LAYOUT::D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//行列リソースを作成
	result = dev->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
		&cbvResDesc,
		D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&resource)
	);

	UINT* data = nullptr;
	result = resource->Map(0, nullptr, (void**)(&data));
	memcpy(data, &mt, sizeof(mt));

	signature->Release();

	ID3DBlob* vs = nullptr;
	ID3DBlob* ps = nullptr;

	result = D3DCompileFromFile(L"Primitive.hlsl", nullptr, nullptr, "VS", "vs_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &vs, &error);
	result = D3DCompileFromFile(L"Primitive.hlsl", nullptr, nullptr, "PS", "ps_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &ps, &error);

	//レイアウト
	D3D12_INPUT_ELEMENT_DESC inputDesc[] = {
		//頂点
		{
			"POSITION",																//SemanticName
			0,																		//SemanticIndex
			DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT,								//Format
			0,																		//InputSlot
			D3D12_APPEND_ALIGNED_ELEMENT,											//AlignedByteoffset
			D3D12_INPUT_CLASSIFICATION::D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, //InputSlotClass
			0																		//InstanceDataStepRate
		},
		//法線
		{
			"NORMAL",
			0,
			DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION::D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		},
		//UV
		{
			"TEXCOORD",
			0,
			DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION::D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		}
	};

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
	gpsDesc.InputLayout.NumElements				= _countof(inputDesc);
	gpsDesc.InputLayout.pInputElementDescs		= inputDesc;
	gpsDesc.pRootSignature						= rs;
	gpsDesc.RasterizerState						= CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	gpsDesc.RasterizerState.CullMode			= D3D12_CULL_MODE::D3D12_CULL_MODE_NONE;
	gpsDesc.RTVFormats[0]						= DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
	gpsDesc.PrimitiveTopologyType				= D3D12_PRIMITIVE_TOPOLOGY_TYPE::D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	gpsDesc.SampleDesc.Count					= 1;
	gpsDesc.NumRenderTargets					= 1;
	gpsDesc.SampleMask							= UINT_MAX;

	result = dev->CreateGraphicsPipelineState(&gpsDesc, IID_PPV_ARGS(&piplineState));
}

void PrimitiveManager::Initialize() {
}


void PrimitiveManager::SetPrimitiveDrawMode(ID3D12GraphicsCommandList* list) {
	list->SetGraphicsRootSignature(rs);
	list->SetPipelineState(piplineState);
	list->SetGraphicsRootConstantBufferView(0, resource->GetGPUVirtualAddress());
}

void PrimitiveManager::Draw(ID3D12GraphicsCommandList* list) {
	for (auto& prim : obj){
		prim->Draw(list);
	}
}

std::shared_ptr<Plane> PrimitiveManager::CreatePlane(const XMFLOAT3& pos, const float width, const float depth) {
	auto ret = std::make_shared<Plane>(dev, pos, width, depth);
	obj.emplace_back(ret);
	return ret;
}


PrimitiveManager::~PrimitiveManager() {
}
