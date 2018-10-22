#pragma once
#include "d3dx12.h"
#include <DirectXMath.h>
#include <DirectXTex/DirectXTex.h>
#include <memory>
#include <vector>

#pragma comment(lib, "DirectXTex.lib")

class PMDLoader;
struct PMDMaterial;

struct Materials {
	DirectX::XMFLOAT4	diffuse;	//�����F
	DirectX::XMFLOAT4	specular;	//����F
	DirectX::XMFLOAT4	ambient;	//���F
	BOOL				texFlag;	//�e�N�X�`������Ȃ��t���O
};

/*
	�q�[�v�̒��g
	0.�}�e���A��
	1.�e�N�X�`��
	2.��Z�e�N�X�`��
	3.���Z�e�N�X�`��
*/
class Model {
public:
	Model();

	void Initialize(ID3D12Device * _dev, DirectX::TexMetadata metaData, std::vector<PMDMaterial> material, UINT textureSize);
	void Draw(ID3D12GraphicsCommandList * _list, ID3D12Device * _dev, std::vector<PMDMaterial> material);
	void WriteToTextureBuffer(DirectX::TexMetadata metaData, uint8_t* img, std::vector<bool> textureFlag);

	//���e�N�X�`������
	void CreateWhiteTexture();
	//���e�N�X�`������
	void CreateBlackTexture();

	~Model();

private:
	//CBV
	std::vector<ID3D12Resource*> materialBuffer;
	//SRV
	std::vector<ID3D12Resource*> textureBuffer;
	ID3D12Resource* whiteBuffer;
	//CBV, SRV�p�q�[�v
	ID3D12DescriptorHeap* heap;
	//�}�e���A��
	Materials mat;
	//�f�[�^
	UINT8* data;
	//�e�N�X�`���t���O
	std::vector<bool> texFlag;

};

