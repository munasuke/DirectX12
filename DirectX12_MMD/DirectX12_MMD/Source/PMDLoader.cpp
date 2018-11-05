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

	//IK
	unsigned short ikNum = 0;
	fread(&ikNum, sizeof(ikNum), 1, fp);
	//飛ばすで
	for (int i = 0; i < ikNum; ++i) {
		fseek(fp, 4, SEEK_CUR);
		unsigned char ikChainNum;
		fread(&ikChainNum, sizeof(ikChainNum), 1, fp);
		fseek(fp, 6, SEEK_CUR);
		fseek(fp, ikChainNum * sizeof(unsigned short), SEEK_CUR);
	}

	//表情
	unsigned short skinNum = 0;
	fread(&skinNum, sizeof(skinNum), 1, fp);
	//飛ばすで
	for (int i = 0; i < skinNum; ++i) {
		fseek(fp, 20, SEEK_CUR);
		unsigned int vertNum = 0;
		fread(&vertNum, sizeof(vertNum), 1, fp);
		fseek(fp, 1, SEEK_CUR);
		fseek(fp, 16 * vertNum, SEEK_CUR);
	}

	//表示用表情
	unsigned char skinDispNum = 0;
	fread(&skinDispNum, sizeof(skinDispNum), 1, fp);
	fseek(fp, skinDispNum * sizeof(unsigned short), SEEK_CUR);

	//表示用ボーン名
	unsigned char boneDispNum = 0;
	fread(&boneDispNum, sizeof(boneDispNum), 1, fp);
	fseek(fp, 50 * boneDispNum, SEEK_CUR);

	//表示ボーンリスト
	unsigned int dispBoneNum = 0;
	fread(&dispBoneNum, sizeof(dispBoneNum), 1, fp);
	fseek(fp, 3 * dispBoneNum, SEEK_CUR);

	//英名対応フラグ
	unsigned char englishFlg = 0;
	fread(&englishFlg, sizeof(englishFlg), 1, fp);
	if (englishFlg) {
		//モデル名20バイト+256バイトコメント
		fseek(fp, 20 + 256, SEEK_CUR);
		//ボーン名20バイト*ボーン数
		fseek(fp, bone.size() * 20, SEEK_CUR);
		//(表情数 - 1) * 20バイト。-1なのはベース部分
		fseek(fp, (skinNum - 1) * 20, SEEK_CUR);
		//ボーン数*50バイト
		fseek(fp, boneDispNum * 50, SEEK_CUR);
	}

	fread(toonTexList.data(), sizeof(char) * 100, toonTexList.size(), fp);

	fclose(fp);

	imageL.lock()->Initialize(material.size());

	//テクスチャ読み込み
	folderName = GetFolderPath(_path);
	for (INT i = 0; i < material.size(); ++i) {
		if (strlen(material[i].textureFilePath) > 0) {
			auto texPath = GetRelativeTexturePathFromPmdPath(_path, material[i].textureFilePath);
			imageL.lock()->Load(texPath.c_str(), material.size(), i);
		}
		//トゥーンテクスチャの読み込み
		if (material[i].toonIndex != 0xff) {
			auto toonFilePath = imageL.lock()->GetToonPathFromIndex(folderName, toonTexList[material[i].toonIndex]);
			imageL.lock()->Load(toonFilePath.c_str(), material.size(), i);
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
