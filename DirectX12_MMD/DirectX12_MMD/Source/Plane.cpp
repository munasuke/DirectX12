#include "Plane.h"
#include <array>

using namespace DirectX;

Plane::Plane(ID3D12Device* dev, const XMFLOAT3& pos, const float width, const float depth) {
	vBuffer = nullptr;
	std::array<PrimitiveVertex, 4> vertex {{
		{XMFLOAT3(pos.x - width / 2.0f, 0.0f, pos.z - depth / 2.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f)},
		{XMFLOAT3(pos.x - width / 2.0f, 0.0f, pos.z + depth / 2.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f)},
		{XMFLOAT3(pos.x - width / 2.0f, 0.0f, pos.z - depth / 2.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f)},
		{XMFLOAT3(pos.x - width / 2.0f, 0.0f, pos.z + depth / 2.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f)},
	}};

	auto result = dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(vertex)),
		D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vBuffer));

	PrimitiveVertex* data = nullptr;
	result = vBuffer->Map(0, nullptr, (void**)&data);
	memcpy(data, &vertex, sizeof(vertex));
	vBuffer->Unmap(0, nullptr);

	//頂点バッファビューの作成
	vbView.BufferLocation	= vBuffer->GetGPUVirtualAddress();	//頂点アドレスのGPUにあるアドレスを記憶
	vbView.StrideInBytes	= sizeof(PrimitiveVertex);			//頂点1つあたりのバイト数を指定
	vbView.SizeInBytes		= sizeof(vertex);					//データ全体のサイズを指定
}

void Plane::Draw(ID3D12GraphicsCommandList* list) {
	list->IASetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	list->IASetVertexBuffers(0, 1, &vbView);
	list->DrawInstanced(4, 1, 0, 0);
}

ID3D12Resource* Plane::GetVBuffer() {
	return vBuffer;
}


Plane::~Plane() {
}
