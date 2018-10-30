#pragma once

#include <DirectXMath.h>
#include <vector>
#include <map>
#include <string>

using namespace DirectX;

struct MotionData {
	unsigned long frameNo;
	//XMFLOAT3 location;
	XMFLOAT4 quaternion;
	//unsigned char  interpolation[64];
	MotionData(){}
	MotionData(unsigned long fram, XMFLOAT4 qua) : frameNo(fram), quaternion(qua){}
};

class VMDMotion {
public:
	VMDMotion();

	int Load(const char* path);
	const std::map<std::string, std::vector<MotionData>>& GetAnimationData() const;

	~VMDMotion();

private:
	unsigned int keyFlameNum;
	std::vector<MotionData> motion;
	std::map<std::string, std::vector<MotionData>> animation;
};

