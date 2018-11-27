#pragma once
#include "d3dx12.h"
#include <DirectXMath.h>
#include <DirectXTex/DirectXTex.h>
#include <memory>
#include <vector>

#pragma comment(lib, "DirectXTex.lib")

//PMD
class PMDLoader;
struct PMDMaterial;
struct BoneNode;

//Texture
class ImageLoader;

//VMD
class VMDMotion;
struct MotionData;

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
	Model(std::shared_ptr<PMDLoader> pmd, std::shared_ptr<ImageLoader> img, std::shared_ptr<VMDMotion> vmd);

	//������
	void Initialize(ID3D12Device * _dev);
	//�X�V
	void Update();
	//�`��
	void Draw(ID3D12GraphicsCommandList * _list, ID3D12Device * _dev);

	//�{�[���o�b�t�@����
	void CreateBoneBuffer(ID3D12Device* dev);

	//�{�[���̍ċA�֐�
	void RecursiveMatrixMultiply(BoneNode* node, DirectX::XMMATRIX& mat);

	//�{�[���̉�]�s��
	void RotationBone(const std::string str, const DirectX::XMFLOAT4& angle, const DirectX::XMFLOAT4& q2 = DirectX::XMFLOAT4(), float t = 0.0f);
	
	//���[�V�����X�V
	void MotionUpdate(int framNo);

	//
	float GetBezierYValueFromXWithNewton(float x, DirectX::XMFLOAT2 a, DirectX::XMFLOAT2 b, unsigned int n = 16);

	//�{�[���o�b�t�@�Ԃ�
	ID3D12Resource* GetBoneBuffer();

	~Model();

private:
	std::weak_ptr<PMDLoader>	pmd;
	std::weak_ptr<ImageLoader>	img;
	std::weak_ptr<VMDMotion>	vmd;

	//�}�e���A���q�[�v
	ID3D12DescriptorHeap* heap;
	//�}�e���A���o�b�t�@
	std::vector<ID3D12Resource*> materialBuffer;
	//�e�N�X�`���o�b�t�@
	std::vector<ID3D12Resource*> textureBuffer;
	//�}�e���A��
	Materials mat;
	//�f�[�^
	UINT8* data;
	//�e�N�X�`���t���O
	std::vector<bool> texFlag;

	//�{�[���o�b�t�@
	ID3D12Resource* boneBuffer;
	//�{�[���s��̃f�[�^
	DirectX::XMMATRIX* boneMatrixData;
};

