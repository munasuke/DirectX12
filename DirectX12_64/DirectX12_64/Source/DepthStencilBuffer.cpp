#include "Result.h"
#include "DepthStencilBuffer.h"
#include "Window.h"

DepthStencilBuffer::DepthStencilBuffer() : depthBuffer(nullptr)
{
}

void DepthStencilBuffer::Initialize(ID3D12Device * _dev, D3D12_DESCRIPTOR_HEAP_DESC _heapDesc)
{
	//深度バッファの作成
	depthResDesc.Dimension			= D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthResDesc.Width				= WIN_WIDTH;													//画面に対して使うバッファ
	depthResDesc.Height				= WIN_HEIGHT;													//画面に対して使うバッファ
	depthResDesc.DepthOrArraySize	= 1;
	depthResDesc.Format				= DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT;
	depthResDesc.SampleDesc.Count	= 1;
	depthResDesc.Flags				= D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	depthHeapProp.Type					= D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_DEFAULT;
	depthHeapProp.CPUPageProperty		= D3D12_CPU_PAGE_PROPERTY::D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	depthHeapProp.MemoryPoolPreference	= D3D12_MEMORY_POOL::D3D12_MEMORY_POOL_UNKNOWN;

	//クリアバリューの作成
	depthClearValue.DepthStencil.Depth	= 1.0f;									//深さ最大値
	depthClearValue.Format				= DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT;	//使うビューと形式を揃える必要がある

	//リソース作成
	result = _dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
		&depthResDesc,
		D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthClearValue,
		IID_PPV_ARGS(&depthBuffer)
	);

	//深度バッファビューの作成
	dsvDesc.Format				= DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT;					//表示形式
	dsvDesc.ViewDimension		= D3D12_DSV_DIMENSION::D3D12_DSV_DIMENSION_TEXTURE2D;	//デプス・ステンシルへのアクセス方法
	dsvDesc.Flags				= D3D12_DSV_FLAGS::D3D12_DSV_FLAG_NONE;					//デプス・ステンシルを読み取り専用にするかのフラグ
	dsvDesc.Texture2D.MipSlice	= 0;													//ミップマップレベルのインデックス

	//ヒープ作成
	heapDesc.Type			= D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	heapDesc.Flags			= D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	heapDesc.NodeMask		= 0;
	heapDesc.NumDescriptors = 1;
	_dev->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&descriptorHeap));
	_dev->CreateDepthStencilView(depthBuffer, &dsvDesc, descriptorHeap->GetCPUDescriptorHandleForHeapStart());
}

void DepthStencilBuffer::SetDescriptor(ID3D12GraphicsCommandList* _list) {
	_list->ClearDepthStencilView(descriptorHeap->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAGS::D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

ID3D12DescriptorHeap * DepthStencilBuffer::GetHeap() {
	return descriptorHeap;
}


DepthStencilBuffer::~DepthStencilBuffer()
{
}
