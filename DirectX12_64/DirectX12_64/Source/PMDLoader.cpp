#include "PMDLoader.h"
#include <tchar.h>
#include <stdio.h>
#include <iostream>

PMDLoader::PMDLoader() : resource(nullptr){
}

int PMDLoader::Load(const char * _path) {
	FILE* fp = nullptr;
	if (fopen_s(&fp, _path, "rb") != 0){
		std::cout << "モデルデータ取得失敗" << std::endl;
		return -1;
	}
	//ヘッダー読み込み
	fread(&header, sizeof(header), 1, fp);
	//fread(&pmd.header.magic, sizeof(pmd.header.magic), 1, fp);
	//fread(&pmd.header.version, sizeof(pmd.header) - sizeof(pmd.header.magic), 1, fp);
	
	//頂点データ読み込み
	vertices.resize(header.vertexNum);
	fread(&vertices[0], sizeof(PMDVertex), header.vertexNum, fp);

	//インデックス総数読み込み
	fread(&indicesNum, sizeof(UINT), 1, fp);
	//インデックスデータ読み込み
	indices.resize(indicesNum);
	fread(&indices[0], sizeof(USHORT), indicesNum, fp);

	//マテリアル総数読み込み
	fread(&materialNum, sizeof(UINT), 1, fp);
	//マテリアルデータ読み込み
	material.resize(materialNum);
	fread(&material[0], sizeof(PMDMaterial), materialNum, fp);

	fclose(fp);

	return 0;
}

void PMDLoader::Initialize(ID3D12Device * _dev) {
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

PMDLoader::~PMDLoader() {
}
