#pragma once

class Device : public Result {
public:
	Device();
	ID3D12Device* GetDevice();//�f�o�C�X��Ԃ�
	~Device();
private:
	ID3D12Device* dev;
};

