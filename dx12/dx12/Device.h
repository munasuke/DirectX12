#pragma once
#include <vector>

class Device : public Result {
public:
	Device();
	ID3D12Device* GetDevice();//�f�o�C�X��Ԃ�
	~Device();
private:
	ID3D12Device * dev;
	//std::vector<IDXGIAdapter*> adapters;
	//IDXGIAdapter* adapter;
};

