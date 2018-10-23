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
	�}�e���A���q�[�v
	0.�}�e���A��
	1.�e�N�X�`��
	2.��Z�e�N�X�`��
	3.���Z�e�N�X�`��

	�{�[���q�[�v
	0.�{�[���s��
*/
class Model {
public:
	Model();

	//������
	void Initialize(ID3D12Device * _dev, DirectX::TexMetadata metaData, std::vector<PMDMaterial> material, UINT textureSize);
	//�`��
	void Draw(ID3D12GraphicsCommandList * _list, ID3D12Device * _dev, std::vector<PMDMaterial> material);
	//�e�N�X�`����������
	void WriteToTextureBuffer(DirectX::TexMetadata metaData, uint8_t* img, std::vector<bool> textureFlag);

	//���e�N�X�`������
	void CreateWhiteTexture();
	//���e�N�X�`������
	void CreateBlackTexture();

	~Model();

private:
	//�}�e���A���q�[�v
	ID3D12DescriptorHeap* heap;
	//�}�e���A���o�b�t�@
	std::vector<ID3D12Resource*> materialBuffer;
	//�e�N�X�`���o�b�t�@
	std::vector<ID3D12Resource*> textureBuffer;
	//���e�N�X�`���o�b�t�@
	ID3D12Resource* whiteBuffer;
	//���e�N�X�`���o�b�t�@
	ID3D12Resource* blackBuffer;
	//�}�e���A��
	Materials mat;
	//�f�[�^
	UINT8* data;
	//�e�N�X�`���t���O
	std::vector<bool> texFlag;

	//�{�[���q�[�v
	ID3D12DescriptorHeap* boneHeap;
	//�{�[���o�b�t�@
	ID3D12Resource* boneBuffer;
	//�{�[���s��̃f�[�^
	DirectX::XMMATRIX* boneMatrixData;
};

