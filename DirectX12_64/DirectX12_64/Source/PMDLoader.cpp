#include "PMDLoader.h"
#include <tchar.h>
#include <stdio.h>
#include <iostream>

PMDLoader::PMDLoader() {
}

int PMDLoader::Load(const char * _path) {
	FILE* fp = nullptr;
	if (fopen_s(&fp, _path, "rb") != 0){
		std::cout << "モデルデータ取得失敗" << std::endl;
		return -1;
	}
	//ヘッダー読み込み
	fread(&pmd.header, sizeof(PMDHeader), 1, fp);
	
	//頂点データ読み込み
	pmd.vertices.resize(pmd.header.vertexNum);
	fread(&pmd.vertices[0], sizeof(PMDVertex), pmd.header.vertexNum, fp);

	//インデックス総数読み込み
	fread(&pmd.indicesNum, sizeof(UINT), 1, fp);
	//インデックスデータ読み込み
	pmd.indices.resize(pmd.indicesNum);
	fread(&pmd.indices[0], sizeof(USHORT), pmd.indicesNum, fp);

	//マテリアル総数読み込み
	fread(&pmd.materialNum, sizeof(UINT), 1, fp);
	//マテリアルデータ読み込み
	pmd.material.resize(pmd.materialNum);
	fread(&pmd.material[0], sizeof(PMDMaterial), pmd.materialNum, fp);

	fclose(fp);

	return 0;
}

PMDHeader PMDLoader::GetPMDHeader() {
	return pmd.header;
}

std::vector<PMDVertex> PMDLoader::GetPMDVertex() {
	return pmd.vertices;
}

std::vector<USHORT> PMDLoader::GetIndices() {
	return pmd.indices;
}

PMDData PMDLoader::GetPMDData() {
	return pmd;
}


PMDLoader::~PMDLoader() {
}
