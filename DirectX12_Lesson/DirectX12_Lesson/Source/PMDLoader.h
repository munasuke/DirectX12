#pragma once
#include <Windows.h>
#include <DirectXMath.h>
#include <vector>

#pragma pack(1)
struct PMDHeader{
	CHAR	magic[3];		//PMD
	FLOAT	version;		//�o�[�W����
	CHAR	name[20];		//���f����
	CHAR	comment[256];	//�R�����g
	UINT	vertexNum;		//���_��
};
#pragma pack()

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

//PMD�ǂݍ��݃N���X
class PMDLoader {
public:
	PMDLoader();

	int Load(const char* _path);
	PMDHeader GetPMDHeader();
	std::vector<PMDVertex> GetPMDVertex();

	~PMDLoader();
private:
	PMDHeader pmdHeader;
	std::vector<PMDVertex> vertices;
};

