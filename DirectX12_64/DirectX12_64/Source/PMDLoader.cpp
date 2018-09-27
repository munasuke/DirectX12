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
	fread(&pmdHeader, sizeof(PMDHeader), 1, fp);
	
	//頂点データ読み込み
	vertices.resize(pmdHeader.vertexNum);
	fread(&vertices[0], sizeof(PMDVertex), pmdHeader.vertexNum, fp);

	//インデックス総数読み込み
	fread(&indicesNum, sizeof(UINT), 1, fp);
	//インデックスデータ読み込み
	indices.resize(indicesNum);
	fread(&indices[0], sizeof(USHORT), indicesNum, fp);

	fclose(fp);

	return 0;
}

PMDHeader PMDLoader::GetPMDHeader() {
	return pmdHeader;
}

std::vector<PMDVertex> PMDLoader::GetPMDVertex() {
	return vertices;
}

std::vector<USHORT> PMDLoader::GetIndices() {
	return indices;
}


PMDLoader::~PMDLoader() {
}
