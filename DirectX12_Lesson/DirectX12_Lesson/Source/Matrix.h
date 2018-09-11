#pragma once
#include <map>

class DirectXMath;

enum VEC{
	Eye,	//���_�x�N�g��
	Target,	//�����_�x�N�g��
	Upper	//��x�N�g��
};

struct Matrixs{
	DirectX::XMMATRIX world;		//���[���h�s��
	DirectX::XMMATRIX camera;		//�r���[�s��
	DirectX::XMMATRIX projection;	//�v���W�F�N�V�����s��
};

class Matrix {
public:
	Matrix();
	~Matrix();
private:
	Matrixs mt;
	std::map<VEC, DirectX::XMVECTOR> vec;//�x�N�g��
};

