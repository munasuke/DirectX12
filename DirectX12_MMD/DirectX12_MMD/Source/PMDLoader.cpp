#include "PMDLoader.h"
#include "BmpLoader.h"
#include "ImageLoader.h"
#include <tchar.h>
#include <stdio.h>
#include <iostream>
#include <string>

PMDLoader::PMDLoader(std::shared_ptr<BmpLoader> bmp, std::shared_ptr<ImageLoader> imageL) :
	textureNum(0)
{
	this->bmp = bmp;
	this->imageL = imageL;
	mat = {};
}

int PMDLoader::Load(const char * _path) {
	std::cout << _path << std::endl;
	FILE* fp = nullptr;
	if (fopen_s(&fp, _path, "rb") != 0) {
		MessageBox(nullptr, TEXT("Can't load PMDFile"), TEXT("Error"), MB_OK);
		return -1;
	}
	//�w�b�_�[�ǂݍ���
	fread(&header, sizeof(header), 1, fp);
	//fread(&pmd.header.magic, sizeof(pmd.header.magic), 1, fp);
	//fread(&pmd.header.version, sizeof(pmd.header) - sizeof(pmd.header.magic), 1, fp);

	//���_���ǂݍ���
	vertices.resize(header.vertexNum);
	fread(&vertices[0], sizeof(PMDVertex), header.vertexNum, fp);

	//�C���f�b�N�X����
	fread(&indicesNum, sizeof(UINT), 1, fp);
	//�C���f�b�N�X���ǂݍ���
	indices.resize(indicesNum);
	fread(&indices[0], sizeof(USHORT), indicesNum, fp);

	//�}�e���A������
	fread(&materialNum, sizeof(UINT), 1, fp);
	//�}�e���A�����ǂݍ���
	material.resize(materialNum);
	fread(&material[0], sizeof(PMDMaterial), materialNum, fp);

	//�{�[������
	fread(&boneCount, sizeof(boneCount), 1, fp);
	//�{�[�����ǂݍ���
	bone.resize(boneCount);
	for (auto& b : bone) {
		fread(&b.boneName, sizeof(b.boneName), 1, fp);
		fread(&b.parentBoneIndex, sizeof(b.parentBoneIndex), 1, fp);
		fread(&b.tailPosBoneIndex, sizeof(b.tailPosBoneIndex), 1, fp);
		fread(&b.boneType, sizeof(b.boneType), 1, fp);
		fread(&b.ikParentBoneIndex, sizeof(b.ikParentBoneIndex), 1, fp);
		fread(&b.boneHeadPos, sizeof(b.boneHeadPos), 1, fp);
	}

	//IK
	unsigned short ikNum = 0;
	fread(&ikNum, sizeof(ikNum), 1, fp);
	//��΂���
	for (int i = 0; i < ikNum; ++i) {
		fseek(fp, 4, SEEK_CUR);
		unsigned char ikChainNum;
		fread(&ikChainNum, sizeof(ikChainNum), 1, fp);
		fseek(fp, 6, SEEK_CUR);
		fseek(fp, ikChainNum * sizeof(unsigned short), SEEK_CUR);
	}

	//�\��
	unsigned short skinNum = 0;
	fread(&skinNum, sizeof(skinNum), 1, fp);
	//��΂���
	for (int i = 0; i < skinNum; ++i) {
		fseek(fp, 20, SEEK_CUR);
		unsigned int vertNum = 0;
		fread(&vertNum, sizeof(vertNum), 1, fp);
		fseek(fp, 1, SEEK_CUR);
		fseek(fp, 16 * vertNum, SEEK_CUR);
	}

	//�\���p�\��
	unsigned char skinDispNum = 0;
	fread(&skinDispNum, sizeof(skinDispNum), 1, fp);
	fseek(fp, skinDispNum * sizeof(unsigned short), SEEK_CUR);

	//�\���p�{�[����
	unsigned char boneDispNum = 0;
	fread(&boneDispNum, sizeof(boneDispNum), 1, fp);
	fseek(fp, 50 * boneDispNum, SEEK_CUR);

	//�\���{�[�����X�g
	unsigned int dispBoneNum = 0;
	fread(&dispBoneNum, sizeof(dispBoneNum), 1, fp);
	fseek(fp, 3 * dispBoneNum, SEEK_CUR);

	//�p���Ή��t���O
	unsigned char englishFlg = 0;
	fread(&englishFlg, sizeof(englishFlg), 1, fp);
	if (englishFlg) {
		//���f����20�o�C�g+256�o�C�g�R�����g
		fseek(fp, 20 + 256, SEEK_CUR);
		//�{�[����20�o�C�g*�{�[����
		fseek(fp, bone.size() * 20, SEEK_CUR);
		//(�\� - 1) * 20�o�C�g�B-1�Ȃ̂̓x�[�X����
		fseek(fp, (skinNum - 1) * 20, SEEK_CUR);
		//�{�[����*50�o�C�g
		fseek(fp, boneDispNum * 50, SEEK_CUR);
	}

	fread(toonTexList.data(), sizeof(char) * 100, toonTexList.size(), fp);

	fclose(fp);

	imageL.lock()->Initialize(material.size());

	//�e�N�X�`���ǂݍ���
	folderName = GetFolderPath(_path);
	for (INT i = 0; i < material.size(); ++i) {
		if (strlen(material[i].textureFilePath) > 0) {
			auto texPath = GetRelativeTexturePathFromPmdPath(_path, material[i].textureFilePath);
			imageL.lock()->Load(texPath.c_str(), material.size(), i);
		}
		//�g�D�[���e�N�X�`���̓ǂݍ���
		if (material[i].toonIndex != 0xff) {
			auto toonFilePath = imageL.lock()->GetToonPathFromIndex(folderName, toonTexList[material[i].toonIndex]);
			imageL.lock()->Load(toonFilePath.c_str(), material.size(), i);
		}
	}

	//�{�[��������
	boneMatrices.resize(bone.size());
	std::fill(boneMatrices.begin(), boneMatrices.end(), DirectX::XMMatrixIdentity());

	//�{�[���m�[�h�}�b�v�̌`��
	for (UINT i = 0; i < bone.size(); ++i) {
		auto& pb = bone[i];
		auto& boneNode = boneMap[pb.boneName];
		boneNode.boneIndex = i;
		boneNode.startPos = pb.boneHeadPos;
	}

	//�{�[���c���[�`��
	for (auto& pb : bone) {
		if (pb.parentBoneIndex >= bone.size()) {
			continue;
		}
		auto pName = bone[pb.parentBoneIndex].boneName;
		boneMap[pName].children.emplace_back(&boneMap[pb.boneName]);
	}

	return 0;
}


PMDHeader PMDLoader::GetPMDHeader() {
	return header;
}

std::vector<PMDVertex> PMDLoader::GetPMDVertex() {
	return vertices;
}

std::vector<USHORT> PMDLoader::GetIndices() {
	return indices;
}

std::vector<PMDMaterial> PMDLoader::GetMaterial() {
	return material;
}

MAT PMDLoader::GetMat() {
	return mat;
}

std::vector<bool> PMDLoader::GetTexFlag()
{
	return texFlag;
}

std::string PMDLoader::GetRelativeTexturePathFromPmdPath(const char * modelPath, const char * texturePath) {
	std::string strModelPath = modelPath;

	auto index1 = strModelPath.rfind('/');
	auto index2 = strModelPath.rfind('\\');
	index2 = std::string::npos ? 0 : index2;

	auto separation = max(index1, index2);

	return strModelPath.substr(0, separation) + "/" + texturePath;
}

std::string PMDLoader::GetFolderPath(const char * modelPath) {
	std::string strModelPath = modelPath;

	auto index1 = strModelPath.rfind('/');
	auto index2 = strModelPath.rfind('\\');
	index2 = std::string::npos ? 0 : index2;

	auto separation = max(index1, index2);

	return strModelPath.substr(0, separation) + "/";
}

UINT PMDLoader::GetTextureNum() {
	return textureNum;
}

std::vector<Bone> PMDLoader::GetBoneData()
{
	return bone;
}


PMDLoader::~PMDLoader() {
}
