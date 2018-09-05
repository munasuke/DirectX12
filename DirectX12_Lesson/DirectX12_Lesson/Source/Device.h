#pragma once
#include "d3dx12.h"

class Device : public Result {
public:
	Device();

	ID3D12Device* dev;
	~Device();
private:
};

