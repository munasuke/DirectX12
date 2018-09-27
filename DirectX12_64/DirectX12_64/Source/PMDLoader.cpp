#include "PMDLoader.h"
#include <tchar.h>
#include <stdio.h>
#include <iostream>

PMDLoader::PMDLoader() {
}

int PMDLoader::Load(const char * _path) {
	FILE* fp = nullptr;
	if (fopen_s(&fp, _path, "rb") != 0){
		std::cout << "���f���f�[�^�擾���s" << std::endl;
		return -1;
	}
	//�w�b�_�[�ǂݍ���
	fread(&pmdHeader, sizeof(PMDHeader), 1, fp);
	
	//���_�f�[�^�ǂݍ���
	vertices.resize(pmdHeader.vertexNum);
	fread(&vertices[0], sizeof(PMDVertex), pmdHeader.vertexNum, fp);

	//�C���f�b�N�X�����ǂݍ���
	fread(&indicesNum, sizeof(UINT), 1, fp);
	//�C���f�b�N�X�f�[�^�ǂݍ���
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
