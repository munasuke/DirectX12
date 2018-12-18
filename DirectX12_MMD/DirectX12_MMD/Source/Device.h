#pragma once
#include <vector>

struct ID3D12Device;
struct IDXGIFactory4;

class Device {
public:
	Device(IDXGIFactory4 * factory);
	void Initialize(IDXGIFactory4* factory);
	ID3D12Device* GetDevice();//�f�o�C�X��Ԃ�
	~Device();
private:
	ID3D12Device* dev;
};

