#pragma once
#include <Windows.h>
#include <vector>

//BMP�摜�̓ǂݍ���
class BmpLoader {
public:
	BmpLoader();
	int Load(const char* _path);//�ǂݍ���
	BITMAPINFOHEADER	GetInfoHeader();
	std::vector<CHAR>	GetData();
	~BmpLoader();
private:
	BITMAPFILEHEADER bmpFileHeader = {};
	BITMAPINFOHEADER bmpInfoHeader = {};
	std::vector<CHAR> data;
};

