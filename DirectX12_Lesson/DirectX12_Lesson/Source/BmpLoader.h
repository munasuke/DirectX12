#pragma once
#include <Windows.h>
#include <vector>

//BMP�摜�̓ǂݍ���
class BmpLoader {
public:
	BmpLoader();
	int Load(const char* _path);//�ǂݍ���
	std::vector<CHAR> GetData();
	~BmpLoader();
private:
	std::vector<CHAR> data;
};

