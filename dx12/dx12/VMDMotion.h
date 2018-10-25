#pragma once

#include <DirectXMath.h>

using namespace DirectX;

struct MotionData {
	char BoneName[15];
	unsigned int frameNo;
	XMFLOAT3 location;
	XMFLOAT4 rotation;
	unsigned char  interpolation[64];
};

class VMDMotion {
public:
	VMDMotion();

	int Load(const char* path);

	~VMDMotion();
};

