#pragma once

class Device : public Result {
public:
	Device();
	ID3D12Device* GetDevice();
	~Device();
private:
	ID3D12Device* dev;
};

