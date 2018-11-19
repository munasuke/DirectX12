#pragma once
#include "d3dx12.h"

class PrimitiveObject {
public:
	PrimitiveObject();

	virtual void Draw() = 0;
	virtual ID3D12Resource* GetVBuffer() = 0;

	virtual ~PrimitiveObject();

protected:
	ID3D12Resource* CreateVBV();
	ID3D12Resource* vBuffer;
};

