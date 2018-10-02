#include "PMDLoader.h"
#include <tchar.h>
#include <stdio.h>
#include <iostream>

PMDLoader::PMDLoader() : resource(nullptr){
}

int PMDLoader::Load(const char * _path) {
	FILE* fp = nullptr;
	if (fopen_s(&fp, _path, "rb") != 0){
		std::cout << "���f���f�[�^�擾���s" << std::endl;
		return -1;
	}
	//�w�b�_�[�ǂݍ���
	fread(&header, sizeof(header), 1, fp);
	//fread(&pmd.header.magic, sizeof(pmd.header.magic), 1, fp);
	//fread(&pmd.header.version, sizeof(pmd.header) - sizeof(pmd.header.magic), 1, fp);
	
	//���_�f�[�^�ǂݍ���
	vertices.resize(header.vertexNum);
	fread(&vertices[0], sizeof(PMDVertex), header.vertexNum, fp);

	//�C���f�b�N�X�����ǂݍ���
	fread(&indicesNum, sizeof(UINT), 1, fp);
	//�C���f�b�N�X�f�[�^�ǂݍ���
	indices.resize(indicesNum);
	fread(&indices[0], sizeof(USHORT), indicesNum, fp);

	//�}�e���A�������ǂݍ���
	fread(&materialNum, sizeof(UINT), 1, fp);
	//�}�e���A���f�[�^�ǂݍ���
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
