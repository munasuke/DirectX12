#include "Camera.h"
#include "Window.h"

using namespace DirectX;

Camera::Camera() : heap(nullptr) {
}

void Camera::Initialize(ID3D12Device * _dev) {
	//視線、注視点、上ベクトル
	XMFLOAT3 eye(0.0f, 10.0f, -15.0f);
	XMFLOAT3 focus(0.0f, 10.0f, 0.0f);
	XMFLOAT3 upper(0.0f, 1.0f, 0.0f);

	//ワールド・ビュー・プロジェクション行列の作成
	mt.world = XMMatrixIdentity();
	mt.view = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&focus), XMLoadFloat3(&upper));
	mt.projection = XMMatrixPerspectiveFovLH(90.0f * 3.14159264f / 180.0f,
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
	auto result = _dev->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
		&cbvResDesc,
		D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&resource)
	);
	//コンスタントバッファビューの設定
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = resource->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = (sizeof(mt) + 0xff) &~0xff;

	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.NumDescriptors = 1;//領域確保
	heapDesc.Type			= D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	heapDesc.Flags			= D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	result = _dev->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&heap));

	_dev->CreateConstantBufferView(&cbvDesc, heap->GetCPUDescriptorHandleForHeapStart());

	result = resource->Map(0, nullptr, (void**)(&data));
	memcpy(data, &mt, sizeof(mt));
}

void Camera::UpdateWVP() {
	static FLOAT angle = 0.0f;
	mt.world = XMMatrixRotationY(angle * 3.14159264f / 180.0f);
	memcpy(data, &mt, sizeof(mt));
	++angle;
}

void Camera::SetDescriptor(ID3D12GraphicsCommandList * _list, ID3D12Device * _dev) {
	_list->SetDescriptorHeaps(1, &heap);
	_list->SetGraphicsRootDescriptorTable(0, heap->GetGPUDescriptorHandleForHeapStart());
}


Camera::~Camera() {
}
