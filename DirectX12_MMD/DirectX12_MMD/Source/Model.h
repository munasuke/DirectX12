#pragma once
#include "d3dx12.h"
#include <DirectXMath.h>
#include <DirectXTex/DirectXTex.h>
#include <memory>
#include <vector>

#pragma comment(lib, "DirectXTex.lib")

class PMDLoader;
struct PMDMaterial;
class ImageLoader;

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
	Model(std::shared_ptr<PMDLoader> pmd, std::shared_ptr<ImageLoader> img);

	//������
	void Initialize(ID3D12Device * _dev);
	//�`��
	void Draw(ID3D12GraphicsCommandList * _list, ID3D12Device * _dev);
	//�e�N�X�`����������
	void WriteToTextureBuffer(std::vector<bool> textureFlag);

	//���e�N�X�`������
	void CreateWhiteTexture();
	//���e�N�X�`������
	void CreateBlackTexture();

	//�{�[���o�b�t�@����
	void CreateBoneBuffer(ID3D12Device* dev);

	~Model();

private:
	std::weak_ptr<PMDLoader>	pmd;
	std::weak_ptr<ImageLoader>	img;

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

