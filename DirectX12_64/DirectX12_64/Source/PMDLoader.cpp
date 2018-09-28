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
	fread(&pmd.header, sizeof(PMDHeader), 1, fp);
	
	//���_�f�[�^�ǂݍ���
	pmd.vertices.resize(pmd.header.vertexNum);
	fread(&pmd.vertices[0], sizeof(PMDVertex), pmd.header.vertexNum, fp);

	//�C���f�b�N�X�����ǂݍ���
	fread(&pmd.indicesNum, sizeof(UINT), 1, fp);
	//�C���f�b�N�X�f�[�^�ǂݍ���
	pmd.indices.resize(pmd.indicesNum);
	fread(&pmd.indices[0], sizeof(USHORT), pmd.indicesNum, fp);

	//�}�e���A�������ǂݍ���
	fread(&pmd.materialNum, sizeof(UINT), 1, fp);
	//�}�e���A���f�[�^�ǂݍ���
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
