#pragma once
#include "d3dx12.h"
#include <Windows.h>
#include <DirectXMath.h>
#include <vector>
#include <memory>

class BmpLoader;
class ImageLoader;

//�w�b�_�[
#pragma pack(1)
struct PMDHeader {
	CHAR	magic[3];		//PMD
	FLOAT	version;		//�o�[�W����
	CHAR	name[20];		//���f����
	CHAR	comment[256];	//�R�����g
	UINT	vertexNum;		//���_��
};
#pragma pack()

//���_�f�[�^
#pragma pack(1)
struct PMDVertex {
	DirectX::XMFLOAT3	pos;	//���W
	DirectX::XMFLOAT3	normal;	//�@��
	DirectX::XMFLOAT2	uv;		//uv

	USHORT	bornNum[2];	//�{�[���ԍ�
	UCHAR	bornWeight;	//�E�G�C�g
	UCHAR	edgeFlag;	//�֊s���t���O
};
#pragma pack()

//�}�e���A���f�[�^
#pragma pack(1)
struct PMDMaterial {
	DirectX::XMFLOAT3	diffuse;			//�����F
	FLOAT				alpha;				//�����F�̕s�����x
	FLOAT				specularPower;		//����F�̋���
	DirectX::XMFLOAT3	specular;			//����F
	DirectX::XMFLOAT3	ambient;			//���F
	UCHAR				toonIndex;			//�g�D�[���̃C���f�b�N�X
	UCHAR				edgeFlag;			//�֊s���t���O
	UINT				indexCount;			//�ʒ��_���i�ʒ��_���X�g�̃f�[�^���j
	CHAR				textureFilePath[20];//�e�N�X�`��������Ƃ��̃e�N�X�`���p�X
};
#pragma pack()

//�{�[�����
struct PMDBone {
	CHAR				boneName[20];		//�{�[����
	USHORT				parentBoneIndex;	//�e�̃{�[���ԍ�
	USHORT				tailPosoneIndex;	//tail�ʒu�̃{�[���ԍ�
	UCHAR				boneType;			//�{�[���̎��
	USHORT				ikParentBoneIndex;	//IK�{�[���ԍ�
	DirectX::XMFLOAT3	boneHeadPos;		//�{�[���̃w�b�h�̈ʒu
};

//GPU�ɂ�����}�e���A�����
struct MAT {
	DirectX::XMFLOAT4	diffuse;	//�����F
	DirectX::XMFLOAT4	specular;	//����F
	DirectX::XMFLOAT4	ambient;	//���F
	BOOL				texFlag;	//�e�N�X�`������Ȃ��t���O
};

//PMD�ǂݍ��݃N���X
class PMDLoader {
public:
	PMDLoader(std::shared_ptr<BmpLoader> bmp, std::shared_ptr<ImageLoader> imageL);

	int Load(const char* _path);

	void SetData(UINT8* data);

	PMDHeader					GetPMDHeader();	//�w�b�_�[����Ԃ�
	std::vector<PMDVertex>		GetPMDVertex();	//���_����Ԃ�
	std::vector<USHORT>			GetIndices();	//�C���f�b�N�X����Ԃ�
	std::vector<PMDMaterial>	GetMaterial();	//�}�e���A������Ԃ�
	MAT							GetMat();
	std::vector<bool>			GetTexFlag();
	/*���f���p�X�ƃe�N�X�`�����΃p�X����A�A�v������e�N�X�`���̃p�X�𐶐�����B
	//���f���̑��΃p�X�i��΃p�X�ł��j
	//�e�N�X�`���̑��΃p�X�i���΃p�X�I�����[�j
	*/
	std::string					GetRelativeTexturePathFromPmdPath(const char* modelPath, const char* texturePath);
	UINT8*						GetData();		//�f�[�^��Ԃ�
	UINT						GetTextureNum();

	void UpdateData();

	~PMDLoader();
private:
	std::weak_ptr<BmpLoader>	bmp;		//BMP
	std::weak_ptr<ImageLoader>	imageL;

	UINT textureNum;

	PMDHeader					header;		//�w�b�_�[
	std::vector<PMDVertex>		vertices;	//���_���
	UINT						indicesNum;	//�C���f�b�N�X����
	std::vector<USHORT>			indices;	//�C���f�b�N�X���
	UINT						materialNum;//�}�e���A������
	std::vector<PMDMaterial>	material;	//�}�e���A�����

	MAT mat;
	std::vector<bool> texFlag;

	ID3D12Resource* resource;
	ID3D12DescriptorHeap* descriptorHeap;
	UINT8* data;
	D3D12_RESOURCE_DESC resourceDesc = {};
	D3D12_HEAP_PROPERTIES heapProp = {};
};

