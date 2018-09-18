#include "Result.h"
#include "ConstantBuffer.h"
#include "Window.h"
#include <iostream>

using namespace DirectX;

ConstantBuffer::ConstantBuffer() :
	constantBuffer(nullptr), cbvDescHeap(nullptr), data(nullptr)
{
	mt = {};
}

void ConstantBuffer::Initialize(ID3D12Device * _dev) {
	//視線、注視点、上ベクトル
	XMVECTOR eye	= { 0.0f, 10.0f, -15.0f };
	XMVECTOR focus	= { 0.0f, 10.0f,	0.0f };
	XMVECTOR upper	= { 0.0f, 1.0f,   0.0f };

	//ワールドビュープロジェクション
	mt.world		= XMMatrixIdentity();
	mt.view			= XMMatrixLookAtLH(eye, focus, upper);
	mt.projection	= XMMatrixPerspectiveFovLH(90.0f * 3.14159264f / 180.0f, static_cast<FLOAT>(WIN_WIDTH) / static_cast<FLOAT>(WIN_HEIGHT), 0.01f, 500.0f);

	//デスクリプタヒープの作成
	cbvHeapDesc.Type			= D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;	//コンスタントバッファ
	cbvHeapDesc.NumDescriptors	= 1;
	cbvHeapDesc.Flags			= D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;							//シェーダから見えるようにする
	cbvHeapDesc.NodeMask		= 0;
	result = _dev->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&cbvDescHeap));

	heapProperties.Type					= D3D12_HEAP_TYPE_UPLOAD;
	heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapProperties.CPUPageProperty		= D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProperties.VisibleNodeMask		= 1;
	heapProperties.CreationNodeMask		= 1;

	//リソース設定
	cbvResDesc.Dimension		= D3D12_RESOURCE_DIMENSION_BUFFER;	//1次元バッファ
	cbvResDesc.Width			= (sizeof(mt) + 0xff) &~ 0xff;		//255アライメント
	cbvResDesc.Height			= 1;								//1次元なので１を設定
	cbvResDesc.DepthOrArraySize = 1;								//深さはないので１を設定
	cbvResDesc.MipLevels		= 1;								//ミップはない
	cbvResDesc.SampleDesc.Count = 1;								//ないと失敗する
	cbvResDesc.Layout			= D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	result = _dev->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&cbvResDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constantBuffer));

	//コンスタントバッファビューの設定
	cbvDesc.BufferLocation	= constantBuffer->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes		= (sizeof(mt) + 0xff) &~ 0xff;

	//定数バッファの作成
	_dev->CreateConstantBufferView(&cbvDesc, cbvDescHeap->GetCPUDescriptorHandleForHeapStart());

	//シェーダに行列を渡す
	//D3D12_RANGE range = {0,sizeof(mt )};
	result = constantBuffer->Map(0, nullptr, (void**)(&data));
	memcpy(data, &mt, sizeof(mt));
	Matrixs a = *(Matrixs*)data;
}


ConstantBuffer::~ConstantBuffer() {
	constantBuffer->Unmap(0, nullptr);
	Release(constantBuffer);
	Release(cbvDescHeap);
}

void ConstantBuffer::UpDataWVP(void) {
	//static FLOAT angle = 0.0f;
	//mt.world = XMMatrixRotationY(angle * 3.14159264f / 180.0f);
	//memcpy(data, &mt, sizeof(mt));
	//++angle;
	
	//std::cout << angle * 3.14159264f / 180.0f << std::endl;
}

void ConstantBuffer::SetDescriptor(ID3D12GraphicsCommandList * _list) {
	_list->SetDescriptorHeaps(1, &cbvDescHeap);
	_list->SetGraphicsRootDescriptorTable(1, cbvDescHeap->GetGPUDescriptorHandleForHeapStart());
}
