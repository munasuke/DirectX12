#pragma once
#include <Windows.h>
#include <vector>

//BMP画像の読み込み
class BmpLoader {
public:
	BmpLoader();
	int Load(const char* _path);//読み込み
	std::vector<CHAR> GetData();
	~BmpLoader();
private:
	std::vector<CHAR> data;
};

