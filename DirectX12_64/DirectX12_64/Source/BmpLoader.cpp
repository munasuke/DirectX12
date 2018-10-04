#include "BmpLoader.h"
#include <tchar.h>
#include <d3d12.h>

BmpLoader::BmpLoader() {
}

int BmpLoader::Load(const char* _path) {
	BITMAPFILEHEADER bmpFileHeader = {};
	BITMAPINFOHEADER bmpInfoHeader = {};
	FILE* fp = nullptr;
	if (fopen_s(&fp, _path, "rb") != 0){
		return -1;
	}
	//bmpFileHeader
	fread(&bmpFileHeader, sizeof(bmpFileHeader), 1, fp);
	//bmpInfoHeader
	fread(&bmpInfoHeader, sizeof(bmpInfoHeader), 1, fp);

	//DWORD rgb[3];
	//fread(&rgb[0], sizeof(rgb), 1, fp);

	data.resize(bmpInfoHeader.biSizeImage);//領域確保
	data.resize(bmpInfoHeader.biHeight * bmpInfoHeader.biWidth * 4);
	//bmpデータをすべて読み込む
	for(INT line = bmpInfoHeader.biHeight - 1; line >= 0; --line) {//下から1ラインずつ上がる
		for (INT count = 0; count < bmpInfoHeader.biWidth * 4; count += 4){//左から右へ
			UINT address = line * bmpInfoHeader.biWidth * 4;
			data[address + count] = 0;
			fread(&data[address + count + 1], sizeof(UCHAR), 3, fp);
		}
	}
	fclose(fp);
	return 0;
}

std::vector<CHAR> BmpLoader::GetData() {
	return data;
}


BmpLoader::~BmpLoader() {
}
