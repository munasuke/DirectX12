#pragma once

#include <DirectXMath.h>
#include <vector>
#include <map>
#include <string>

using namespace DirectX;

struct MotionData {
	unsigned int frameNo;
	XMFLOAT3 location;
	XMFLOAT4 quaternion;
	//unsigned char  interpolation[64];
	XMFLOAT2 bz[2];
	MotionData(){}
	MotionData(unsigned int fram, XMFLOAT3& loc, XMFLOAT4& qua, 
		const unsigned char ax, const unsigned char ay,
		const unsigned char bx, const unsigned char by) : frameNo(fram), quaternion(qua) {
		bz[0] = XMFLOAT2(static_cast<float>(ax) / 127.0f, static_cast<float>(ay) / 127.0f);
		bz[1] = XMFLOAT2(static_cast<float>(bx) / 127.0f, static_cast<float>(by) / 127.0f);
	}
};

class VMDMotion {
public:
	VMDMotion();

	int Load(const char* path);
	const std::map<std::string, std::vector<MotionData>>& GetAnimationData() const;
	unsigned int GetDuration() const;

	~VMDMotion();

private:
	unsigned int keyFlameNum;
	std::vector<MotionData> motion;
	std::map<std::string, std::vector<MotionData>> animation;

	//ëçéûä‘
	unsigned int duration;
};

