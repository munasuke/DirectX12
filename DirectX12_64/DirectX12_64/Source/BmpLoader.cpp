#include "BmpLoader.h"
#include <tchar.h>
#include <d3d12.h>

BmpLoader::BmpLoader() {
}

int BmpLoader::Load(const char* _path) {
	FILE* fp = nullptr;
	if (fopen_s(&fp, _path, "rb") != 0){
		MessageBox(nullptr, TEXT("Can't load BMPFile"), TEXT("Error"), MB_OK);
		return -1;
	}
	//bmpFileHeader
	fread(&bmpFileHeader, sizeof(bmpFileHeader), 1, fp);
	//bmpInfoHeader
	fread(&bmpInfoHeader, sizeof(bmpInfoHeader), 1, fp);

	//�r�b�g�̐[���ŃT�C�Y�����߂�
	auto bitCount = bmpInfoHeader.biBitCount == 32 ? bmpInfoHeader.biSizeImage : bmpInfoHeader.biWidth * bmpInfoHeader.biHeight * 4;
	data.resize(bitCount);
	//bmp�f�[�^�����ׂēǂݍ���
	if (bmpInfoHeader.biBitCount == 32) {
		for (INT line = bmpInfoHeader.biHeight - 1; line >= 0; --line) {//������1���C�����オ��
			for (INT count = 0; count < bmpInfoHeader.biWidth * 4; count += 4) {//������E��
				UINT address = line * bmpInfoHeader.biWidth * 4;
				fread(&data[address + count], sizeof(UCHAR), 4, fp);
			}
		}
	}
	else if (bmpInfoHeader.biBitCount == 24) {
		for (INT line = bmpInfoHeader.biHeight - 1; line >= 0; --line) {//������1���C�����オ��
			for (INT count = 0; count < bmpInfoHeader.biWidth * 4; count += 4) {//������E��
				UINT address = line * bmpInfoHeader.biWidth * 4;
				data[address + count] = 0;
				fread(&data[address + count + 1], sizeof(UCHAR), 3, fp);
			}
		}
	}
	fclose(fp);
	return 0;
}

BITMAPINFOHEADER BmpLoader::GetInfoHeader() {
	return bmpInfoHeader;
}

std::vector<CHAR> BmpLoader::GetData() {
	return data;
}


BmpLoader::~BmpLoader() {
}
