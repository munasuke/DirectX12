#pragma once
#include "d3dx12.h"
#include <DirectXMath.h>
#include <DirectXTex/DirectXTex.h>
#include <memory>

#pragma comment(lib, "DirectXTex.lib")

class Window;

//��ԍs��
struct Matrixs {
	DirectX::XMMATRIX world;		//���[���h�s��
	DirectX::XMMATRIX view;			//�r���[�s��
	DirectX::XMMATRIX projection;	//�v���W�F�N�V�����s��
};

/*
	�}�e���A��
	�e�N�X�`��
	��Z�e�N�X�`��
	���Z�e�N�X�`��
*/
class Model {
public:
	Model();

	void Initialize(ID3D12Device * _dev, DirectX::TexMetadata metaData, UINT size, UINT materialSize);

	void CreateWhiteTexture();
	void CreateBlackTexture();

	~Model();

private:
	//�e�N�X�`���o�b�t�@
	ID3D12Resource* textureBuffer;
	//CBV
	ID3D12Resource* resource;
	//CBV, SRV�p�q�[�v
	ID3D12DescriptorHeap* heap;
	//�s��
	Matrixs matrix;

};

