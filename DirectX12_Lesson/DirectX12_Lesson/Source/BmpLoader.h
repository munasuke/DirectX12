#pragma once
#include <string>

//BMP‰æ‘œ‚Ì“Ç‚İ‚İ
class BmpLoader {
public:
	BmpLoader();
	int Load(const char* _path);//“Ç‚İ‚İ
	~BmpLoader();
};

