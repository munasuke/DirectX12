#pragma once
#include "d3dx12.h"
#include <DirectXMath.h>

//空間行列
struct Matrix {
	DirectX::XMMATRIX world;		//ワールド行列
	DirectX::XMMATRIX view;			//ビュー行列
	DirectX::XMMATRIX projection;	//プロジェクション行列
	DirectX::XMMATRIX lvp;			//ライトビュー行列
};

class Camera {
public:
	Camera();

	void Initialize(ID3D12Device* _dev);
	void UpdateWVP();
	void SetDescriptor(ID3D12GraphicsCommandList* _list, ID3D12Device* _dev);
	float& GetRotaSpeed();
	float angle;
	float rotationSpeed;

	~Camera();
private:
	float Magnitude(const DirectX::XMFLOAT3& f);
	DirectX::XMFLOAT3 Normalize(const DirectX::XMFLOAT3& f);

	ID3D12Resource* resource;

	Matrix mt;
	UINT* data;
};
DirectX::XMFLOAT3 operator*(DirectX::XMFLOAT3& v, float f);
DirectX::XMFLOAT3 operator+(DirectX::XMFLOAT3& f1, DirectX::XMFLOAT3& f2);
DirectX::XMFLOAT3 operator-(DirectX::XMFLOAT3& f1, DirectX::XMFLOAT3& f2);
