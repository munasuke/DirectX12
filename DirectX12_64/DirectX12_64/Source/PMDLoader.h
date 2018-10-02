#pragma once
#include "d3dx12.h"
#include <Windows.h>
#include <DirectXMath.h>
#include <vector>


//�w�b�_�[
#pragma pack(1)
struct PMDHeader{
	CHAR	magic[3];		//PMD
	FLOAT	version;		//�o�[�W����
	CHAR	name[20];		//���f����
	CHAR	comment[256];	//�R�����g
	UINT	vertexNum;		//���_��
};
#pragma pack()

//���_�f�[�^
#pragma pack(1)
struct PMDVertex{
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
struct PMDMaterial{
	DirectX::XMFLOAT3	diffuse;			//��{�F(�g�U���ːF)
	FLOAT				alpha;				//�A���t�@�F
	FLOAT				specularPower;		//�X�؃L�����̋���
	DirectX::XMFLOAT3	specular;			//�X�y�L����(���ːF)
	DirectX::XMFLOAT3	ambient;			//�A���r�G���g
	UCHAR				toonIndex;			//�g�D�[���̃C���f�b�N�X
	UCHAR				edgeFlag;			//�֊s���t���O
	UINT				indexCount;			//�C���f�b�N�X��
	CHAR				textureFilePath[20];//�e�N�X�`��������Ƃ��̃e�N�X�`���p�X
};
#pragma pack()

//PMD�ǂݍ��݃N���X
class PMDLoader {
public:
	PMDLoader();

	int Load(const char* _path);

	void Initialize(ID3D12Device* _dev);

	PMDHeader GetPMDHeader();
	std::vector<PMDVertex> GetPMDVertex();
	std::vector<USHORT> GetIndices();		//�C���f�b�N�X����Ԃ�
	std::vector<PMDMaterial> GetMaterial();

	~PMDLoader();
private:
	PMDHeader					header;		//�w�b�_�[
	std::vector<PMDVertex>		vertices;	//���_���
	UINT						indicesNum;	//�C���f�b�N�X����
	std::vector<USHORT>			indices;	//�C���f�b�N�X���
	UINT						materialNum;//�}�e���A������
	std::vector<PMDMaterial>	material;	//�}�e���A�����

	ID3D12Resource* resource;
	D3D12_RESOURCE_DESC resourceDesc = {};
	D3D12_HEAP_PROPERTIES heapProp = {};
};

