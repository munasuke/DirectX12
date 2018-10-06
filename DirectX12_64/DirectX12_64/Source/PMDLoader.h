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

struct MAT{
	DirectX::XMFLOAT3	diffuse;			//�����F
	BOOL texFlag;
};

//PMD�ǂݍ��݃N���X
class PMDLoader {
public:
	PMDLoader();

	int Load(const char* _path);

	void Initialize(ID3D12Device* _dev);
	void Draw(ID3D12GraphicsCommandList * _list, ID3D12Device * _dev, ID3D12DescriptorHeap* texHeap);

	void SetDescriptor(ID3D12GraphicsCommandList * _list, ID3D12Device* _dev);
	void SetDescriptor(ID3D12GraphicsCommandList * _list, ID3D12Device* _dev, UINT index);
	void SetMaterialColor(UINT index);
	void SetData(UINT8* data);

	PMDHeader					GetPMDHeader();	//�w�b�_�[����Ԃ�
	std::vector<PMDVertex>		GetPMDVertex();	//���_����Ԃ�
	std::vector<USHORT>			GetIndices();	//�C���f�b�N�X����Ԃ�
	std::vector<PMDMaterial>	GetMaterial();	//�}�e���A������Ԃ�
	MAT							GetMat();
	std::vector<BOOL>			GetTexFlag();
	UINT8*						GetData();		//�f�[�^��Ԃ�

	void UpdateData();

	~PMDLoader();
private:
	PMDHeader					header;		//�w�b�_�[
	std::vector<PMDVertex>		vertices;	//���_���
	UINT						indicesNum;	//�C���f�b�N�X����
	std::vector<USHORT>			indices;	//�C���f�b�N�X���
	UINT						materialNum;//�}�e���A������
	std::vector<PMDMaterial>	material;	//�}�e���A�����

	MAT mat;
	std::vector<BOOL> texFlag;

	ID3D12Resource* resource;
	ID3D12DescriptorHeap* descriptorHeap;
	UINT8* data;
	D3D12_RESOURCE_DESC resourceDesc = {};
	D3D12_HEAP_PROPERTIES heapProp = {};
};

