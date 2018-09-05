#pragma once

#include "d3dx12.h"

class Device {
public:
	Device();

	ID3D12Device* dev;
	HRESULT result;

	~Device();
private:
};

