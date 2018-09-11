#include "Result.h"
#include "ConstantBuffer.h"
#include "Window.h"

using namespace DirectX;

ConstantBuffer::ConstantBuffer() :
	constantBuffer(nullptr), cbvDescHeap(nullptr), data(nullptr)
{
	mt = {};
}

void ConstantBuffer::Initialize(ID3D12Device * _dev) {
	//視線、注視点、上ベクトル
	XMVECTOR eye	= { 0.0f, 0.0f, -10.0f };
	XMVECTOR focus	= { 0.0f, 0.0f,	  0.0f };
	XMVECTOR upper	= { 0.0f, 1.0f,   0.0f };
	//ワールドビュープロジェクション
	mt.world	  = XMMatrixIdentity();
	mt.camera	  = XMMatrixLookAtLH(eye, focus, upper);
	mt.projection = XMMatrixPerspectiveFovLH(90.0f * 3.14159264f / 180.0f, (float)WIN_HEIGHT / (float)WIN_WIDTH, 0.5f, 500.0f);

	//デスクリプタヒープの作成
	cbvHeapDesc.Type			= D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;//コンスタントバッファ
	cbvHeapDesc.NumDescriptors	= 1;
	cbvHeapDesc.Flags			= D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;//シェーダから見えるようにする
	cbvHeapDesc.NodeMask		= 0;
	result = _dev->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&cbvDescHeap));

	heapProperties.Type					= D3D12_HEAP_TYPE_UPLOAD;
	heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapProperties.CPUPageProperty		= D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProperties.VisibleNodeMask		= 1;
	heapProperties.CreationNodeMask		= 1;

	//リソース設定
	cbvResDesc.Dimension		= D3D12_RESOURCE_DIMENSION_BUFFER;	//1次元バッファ
	cbvResDesc.Width			= (sizeof(XMMATRIX) + 0xff) &~ 0xff;	//255アライメント
	cbvResDesc.Height			= 1;								//1次元なので１を設定
	cbvResDesc.DepthOrArraySize = 1;								//深さはないので１を設定
	cbvResDesc.MipLevels		= 1;								//ミップはない
	cbvResDesc.SampleDesc.Count = 1;								//ないと怒られる
	cbvResDesc.Layout			= D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	result = _dev->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&cbvResDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constantBuffer));

	//コンスタントバッファビューの設定
	cbvDesc.BufferLocation = constantBuffer->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = (sizeof(XMMATRIX) + 0xff) &~ 0xff;

	//定数バッファの作成
	_dev->CreateConstantBufferView(&cbvDesc, cbvDescHeap->GetCPUDescriptorHandleForHeapStart());


	//シェーダに行列を渡す
	result = constantBuffer->Map(0, nullptr, (void**)data);
	memcpy(data, &mt, sizeof(mt));
}


ConstantBuffer::~ConstantBuffer() {
}
