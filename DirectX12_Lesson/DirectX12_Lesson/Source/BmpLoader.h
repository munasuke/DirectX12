#pragma once
#include <string>

//BMP�摜�̓ǂݍ���
class BmpLoader {
public:
	BmpLoader();
	int Load(const char* _path);//�ǂݍ���
	~BmpLoader();
};

