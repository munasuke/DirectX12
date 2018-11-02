#pragma once
#include "d3dx12.h"
#include <DirectXMath.h>

//��ԍs��
struct Matrix {
	DirectX::XMMATRIX world;		//���[���h�s��
	DirectX::XMMATRIX view;			//�r���[�s��
	DirectX::XMMATRIX projection;	//�v���W�F�N�V�����s��
};

class Camera {
public:
	Camera();

	void Initialize(ID3D12Device* _dev);
	void UpdateWVP();
	void SetDescriptor(ID3D12GraphicsCommandList* _list, ID3D12Device* _dev);

	~Camera();
private:
	ID3D12Resource* resource;

	Matrix mt;
	UINT* data;
};

