#pragma once
#include "PrimitiveObject.h"

class Plane :
	public PrimitiveObject {
public:
	Plane(ID3D12Device* dev, const XMFLOAT3& pos, const float width, const float depth, const XMFLOAT4& color);

	void Draw(ID3D12GraphicsCommandList* list);
	ID3D12Resource* GetVBuffer();

	~Plane();
private:
};

