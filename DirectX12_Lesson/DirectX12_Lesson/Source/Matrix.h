#pragma once
#include <DirectXMath.h>

//空間行列
struct Matrixs{
	DirectX::XMMATRIX world;		//ワールド行列
	DirectX::XMMATRIX camera;		//ビュー行列
	DirectX::XMMATRIX projection;	//プロジェクション行列
};

