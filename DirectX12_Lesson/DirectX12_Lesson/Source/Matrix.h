#pragma once
#include <map>

class DirectXMath;

enum VEC{
	Eye,	//視点ベクトル
	Target,	//注視点ベクトル
	Upper	//上ベクトル
};

struct Matrixs{
	DirectX::XMMATRIX world;		//ワールド行列
	DirectX::XMMATRIX camera;		//ビュー行列
	DirectX::XMMATRIX projection;	//プロジェクション行列
};

class Matrix {
public:
	Matrix();
	~Matrix();
private:
	Matrixs mt;
	std::map<VEC, DirectX::XMVECTOR> vec;//ベクトル
};

