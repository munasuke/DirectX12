#include "PMDLoader.h"
#include "BmpLoader.h"
#include "ImageLoader.h"
#include <tchar.h>
#include <stdio.h>
#include <iostream>
#include <string>

PMDLoader::PMDLoader(std::shared_ptr<BmpLoader> bmp, std::shared_ptr<ImageLoader> imageL) :
	textureNum(0),
	resource(nullptr),
	descriptorHeap(nullptr),
	data(nullptr)
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
	//ヘッダー読み込み
	fread(&header, sizeof(header), 1, fp);
	//fread(&pmd.header.magic, sizeof(pmd.header.magic), 1, fp);
	//fread(&pmd.header.version, sizeof(pmd.header) - sizeof(pmd.header.magic), 1, fp);

	//頂点情報読み込み
	vertices.resize(header.vertexNum);
	fread(&vertices[0], sizeof(PMDVertex), header.vertexNum, fp);

	//インデックス総数
	fread(&indicesNum, sizeof(UINT), 1, fp);
	//インデックス情報読み込み
	indices.resize(indicesNum);
	fread(&indices[0], sizeof(USHORT), indicesNum, fp);

	//マテリアル総数
	fread(&materialNum, sizeof(UINT), 1, fp);
	//マテリアル情報読み込み
	material.resize(materialNum);
	fread(&material[0], sizeof(PMDMaterial), materialNum, fp);

	//ボーン総数
	fread(&boneCount, sizeof(boneCount), 1, fp);
	//ボーン情報読み込み
	bone.resize(boneCount);
	for (auto& b : bone) {
		fread(&b.boneName, sizeof(b.boneName), 1, fp);
		fread(&b.parentBoneIndex, sizeof(b.parentBoneIndex), 1, fp);
		fread(&b.tailPosBoneIndex, sizeof(b.tailPosBoneIndex), 1, fp);
		fread(&b.boneType, sizeof(b.boneType), 1, fp);
		fread(&b.ikParentBoneIndex, sizeof(b.ikParentBoneIndex), 1, fp);
		fread(&b.boneHeadPos, sizeof(b.boneHeadPos), 1, fp);
	}

	fclose(fp);

	for (INT i = 0; i < material.size(); ++i) {
		if (strlen(material[i].textureFilePath) > 0) {
			++textureNum;
			auto texPath = GetRelativeTexturePathFromPmdPath(_path, material[i].textureFilePath);
			imageL.lock()->Load(texPath.c_str());
		}
	}

	//ボーン初期化
	boneMatrices.resize(bone.size());
	std::fill(boneMatrices.begin(), boneMatrices.end(), DirectX::XMMatrixIdentity());

	//ボーンノードマップの形成
	for (UINT i = 0; i < bone.size(); ++i) {
		auto& pb = bone[i];
		auto& boneNode = boneMap[pb.boneName];
		boneNode.boneIndex = i;
		boneNode.startPos = pb.boneHeadPos;
	}

	//ボーンツリー形成
	for (auto& pb : bone) {
		if (pb.parentBoneIndex >= bone.size()) {
			continue;
		}
		auto pName = bone[pb.parentBoneIndex].boneName;
		boneMap[pName].children.emplace_back(&boneMap[pb.boneName]);
	}

	return 0;
}

void PMDLoader::SetData(UINT8 * data) {
	this->data = data;
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

	auto textureFilePath = strModelPath.substr(0, separation) + "/" + texturePath;

	return textureFilePath;
}

UINT8 * PMDLoader::GetData(void) {
	return data;
}

UINT PMDLoader::GetTextureNum() {
	return textureNum;
}

void PMDLoader::UpdateData() {
	data = (UINT8*)(((sizeof(DirectX::XMFLOAT3) + 0xff) &~0xff) + (CHAR*)(data));
}

PMDLoader::~PMDLoader() {
}
