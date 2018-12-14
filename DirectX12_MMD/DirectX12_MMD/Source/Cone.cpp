#include "Cone.h"


Cone::Cone(ID3D12Device* dev, const XMFLOAT3& pos, const unsigned int div, const float r, const float height, const XMFLOAT4& color) {
	vBuffer = nullptr;
	vertices.resize(div * 2 + 2);
	for (int i = 0; i <= div; ++i) {
		vertices[i * 2].pos.x = pos.x + r * cos((XM_2PI / static_cast<float>(div)) * static_cast<float>(i));
		vertices[i * 2].pos.y = pos.y;
		vertices[i * 2].pos.z = pos.z + r * sin((XM_2PI / static_cast<float>(div)) * static_cast<float>(i));

		auto normal = vertices[i * 2].pos;
		XMStoreFloat3(&vertices[i * 2].normal, XMVector3Normalize(XMLoadFloat3(&normal)));

		vertices[i * 2].uv.x = (1.0f / static_cast<float>(div)) * static_cast<float>(i);
		vertices[i * 2].uv.y = 1.0f;

		vertices[i * 2 + 1].pos.x = pos.x;
		vertices[i * 2 + 1].pos.y = pos.y + height;
		vertices[i * 2 + 1].pos.z = pos.z;

		vertices[i * 2 + 1].normal = vertices[i * 2].normal;

		vertices[i * 2 + 1].uv.x = (1.0f / static_cast<float>(div)) * static_cast<float>(i);
		vertices[i * 2 + 1].uv.y = 0.0f;
	}

	for (auto& v : vertices) {
		v.color = color;
	}

	//頂点バッファの生成
	auto result = dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(PrimitiveVertex) * vertices.size()),
		D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vBuffer));

	//シェーダに渡す
	PrimitiveVertex* data = nullptr;
	result = vBuffer->Map(0, nullptr, (void**)&data);
	memcpy(data, vertices.data(), sizeof(PrimitiveVertex) * vertices.size());
	vBuffer->Unmap(0, nullptr);

	//頂点バッファビューの作成
	vbView = {};
	vbView.BufferLocation	= vBuffer->GetGPUVirtualAddress();
	vbView.StrideInBytes	= sizeof(PrimitiveVertex);
	vbView.SizeInBytes		= sizeof(PrimitiveVertex) * vertices.size();
}

void Cone::Draw(ID3D12GraphicsCommandList * list) {
	list->IASetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	list->IASetVertexBuffers(0, 1, &vbView);
	list->DrawInstanced(vertices.size(), 1, 0, 0);
}

void Cone::SetVertices(ID3D12GraphicsCommandList * list) {
	list->IASetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	list->IASetVertexBuffers(0, 1, &vbView);
}

ID3D12Resource * Cone::GetVBuffer() {
	return vBuffer;
}

D3D12_VERTEX_BUFFER_VIEW Cone::GetVBV() {
	return vbView;
}


Cone::~Cone() {
}
