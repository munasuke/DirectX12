#pragma once
#include "PrimitiveObject.h"
class Plane :
	public PrimitiveObject {
public:
	Plane(const float width, const float depth, const float nx, const float ny, const float nz);

	void Draw();
	ID3D12Resource* GetVBuffer();

	~Plane();
};

