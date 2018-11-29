#pragma once
#include "d3dx12.h"
#include <DirectXMath.h>

using namespace DirectX;

struct PrimitiveVertex {
	XMFLOAT3 pos;
	XMFLOAT3 normal;
	XMFLOAT2 uv;
	XMFLOAT4 color;

	PrimitiveVertex() {
		pos		= XMFLOAT3(0.0f, 0.0f, 0.0f);
		normal	= XMFLOAT3(0.0f, 0.0f, 0.0f);
		uv		= XMFLOAT2(0.0f, 0.0f);
		color	= XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	}
	PrimitiveVertex(const XMFLOAT3& pos, const XMFLOAT3& normal, const XMFLOAT2& uv, const XMFLOAT4& color) {
		this->pos		= pos;
		this->normal	= normal;
		this->uv		= uv;
		this->color		= color;
	}
	PrimitiveVertex(const float x, const float y, const float z, 
		const float nx, const float ny, const float nz, const float u, const float v) {
		pos		= XMFLOAT3(x, y, z);
		normal	= XMFLOAT3(nx, ny, nz);
		uv		= XMFLOAT2(u, v);
	}
};

class PrimitiveObject {
public:
	PrimitiveObject();

	virtual void Draw(ID3D12GraphicsCommandList* list) = 0;
	virtual ID3D12Resource* GetVBuffer() = 0;

	virtual ~PrimitiveObject();

protected:
	ID3D12Resource* CreateVBV(ID3D12Device* dev);
	ID3D12Resource* vBuffer;
	D3D12_VERTEX_BUFFER_VIEW vbView;
};

