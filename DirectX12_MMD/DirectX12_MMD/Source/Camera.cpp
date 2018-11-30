#include "Camera.h"
#include "Window.h"

using namespace DirectX;

Camera::Camera() {
}

void Camera::Initialize(ID3D12Device * _dev) {
	//視線、注視点、上ベクトル
	XMFLOAT3 eye	(0.0f, 14.0f, -13.0f);
	XMFLOAT3 focus	(0.0f, 12.0f,   0.0f);
	XMFLOAT3 upper	(0.0f,  1.0f,   0.0f);

	//光線ベクトル
	auto toLight = Normalize(eye - focus);
	//光源座標
	auto lightPos = toLight * Magnitude(focus - eye);
	//ライトビュー行列
	auto lightView = XMMatrixLookAtLH(XMLoadFloat3(&lightPos), XMLoadFloat3(&focus), XMLoadFloat3(&upper));
	//ライトプロジェクション
	auto lightProj = XMMatrixOrthographicLH(40.0f, 40.0f, 0.01f, 500.0f);
	//ライトビュープロジェクション
	mt.lvp = lightView * lightProj;

	//ワールド・ビュー・プロジェクション行列の作成
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
	auto result = _dev->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
		&cbvResDesc,
		D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&resource)
	);

	result = resource->Map(0, nullptr, (void**)(&data));
	memcpy(data, &mt, sizeof(mt));
}

void Camera::UpdateWVP() {
	static FLOAT angle = 0.0f;
	mt.world = XMMatrixRotationY(angle * DirectX::XM_PI / 180.0f);
	memcpy(data, &mt, sizeof(mt));
	++angle;
}

void Camera::SetDescriptor(ID3D12GraphicsCommandList * _list, ID3D12Device * _dev) {
	_list->SetGraphicsRootConstantBufferView(0, resource->GetGPUVirtualAddress());
}


Camera::~Camera() {
}

float Camera::Magnitude(const XMFLOAT3 & f) {
	return sqrt(f.x*f.x + f.y*f.y + f.z*f.z);
}

XMFLOAT3 Camera::Normalize(const XMFLOAT3 & f) {
	auto size = Magnitude(f);
	return XMFLOAT3(f.x / size, f.y / size, f.z / size);
}

XMFLOAT3 operator*(XMFLOAT3 & v, float f) {
	return XMFLOAT3(v.x*f, v.y*f, v.z*f);
}

XMFLOAT3 operator-(XMFLOAT3 & f1, XMFLOAT3 & f2) {
	return XMFLOAT3(f1.x - f2.x, f1.y - f2.y, f1.z - f2.z);
}
