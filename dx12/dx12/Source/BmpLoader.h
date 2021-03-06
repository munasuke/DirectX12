#pragma once
#include <Windows.h>
#include <vector>

//BMP画像の読み込み
class BmpLoader {
public:
	BmpLoader();
	int Load(const char* _path);//読み込み
	BITMAPINFOHEADER	GetInfoHeader();
	std::vector<CHAR>	GetData();
	~BmpLoader();
private:
	BITMAPFILEHEADER bmpFileHeader = {};
	BITMAPINFOHEADER bmpInfoHeader = {};
	std::vector<CHAR> data;
};

