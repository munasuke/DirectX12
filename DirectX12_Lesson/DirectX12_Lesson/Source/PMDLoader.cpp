#include "PMDLoader.h"
#include <tchar.h>
#include <stdio.h>

PMDLoader::PMDLoader() {
}

int PMDLoader::Load(const char * _path) {
	FILE* fp = nullptr;
	if (fopen_s(&fp, _path, "rb") != 0){
		return -1;
	}

	fread(&pmdHeader, sizeof(PMDHeader), 1, fp);
	
	vertices.resize(pmdHeader.vertexNum);
	fread(&vertices[0], pmdHeader.vertexNum, pmdHeader.vertexNum, fp);

	fclose(fp);

	return 0;
}

PMDHeader PMDLoader::GetPMDHeader() {
	return pmdHeader;
}

std::vector<PMDVertex> PMDLoader::GetPMDVertex() {
	return vertices;
}


PMDLoader::~PMDLoader() {
}
