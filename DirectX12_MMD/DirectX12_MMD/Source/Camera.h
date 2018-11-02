#pragma once
#include "d3dx12.h"
#include <DirectXMath.h>

//空間行列
struct Matrix {
	DirectX::XMMATRIX world;		//ワールド行列
	DirectX::XMMATRIX view;			//ビュー行列
	DirectX::XMMATRIX projection;	//プロジェクション行列
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

