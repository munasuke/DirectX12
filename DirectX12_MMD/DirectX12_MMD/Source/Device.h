#pragma once
#include <vector>

struct ID3D12Device;

class Device {
public:
	Device();
	ID3D12Device* GetDevice();//�f�o�C�X��Ԃ�
	~Device();
private:
	ID3D12Device* dev;
	//std::vector<IDXGIAdapter*> adapters;
	//IDXGIAdapter* adapter;
};

