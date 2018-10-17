#pragma once
#include "d3dx12.h"
#include <DirectXMath.h>
#include <DirectXTex/DirectXTex.h>
#include <memory>
#include <vector>

#pragma comment(lib, "DirectXTex.lib")

class PMDLoader;

struct Material {
	DirectX::XMFLOAT4	diffuse;	//�����F
	DirectX::XMFLOAT4	specular;	//����F
	DirectX::XMFLOAT4	ambient;	//���F
	BOOL				texFlag;	//�e�N�X�`������Ȃ��t���O
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

	void Initialize(ID3D12Device * _dev, DirectX::TexMetadata metaData, UINT size, std::vector<PMDMaterial> material);

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
	Material mat;
	//�f�[�^
	UINT8* data;
	//�e�N�X�`���t���O
	std::vector<bool> texFlag;

};

