#pragma once
#include <Windows.h>
#include <vector>

//BMP‰æ‘œ‚Ì“Ç‚İ‚İ
class BmpLoader {
public:
	BmpLoader();
	int Load(const char* _path);//“Ç‚İ‚İ
	std::vector<CHAR> GetData();
	~BmpLoader();
private:
	std::vector<CHAR> data;
};

