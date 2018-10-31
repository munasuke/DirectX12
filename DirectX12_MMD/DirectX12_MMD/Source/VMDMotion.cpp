#include "VMDMotion.h"
#include <iostream>
#include <Windows.h>
#include <algorithm>


VMDMotion::VMDMotion() : duration(0){
}

int VMDMotion::Load(const char * path) {
	FILE* fp = nullptr;
	if (fopen_s(&fp, path, "rb") != 0) {
		MessageBox(nullptr, TEXT("Can't load VMDFile"), TEXT("Error"), MB_OK);
		return -1;
	}

	//最初の50バイトは無駄なデータなので飛ばす
	fseek(fp, 50, SEEK_SET);

	//キーフレーム数
	fread(&keyFlameNum, sizeof(keyFlameNum), 1, fp);

	//キーフレームデータ
	std::string name = "";
	name.resize(15);
	motion.resize(keyFlameNum);
	XMFLOAT3 location;
	std::vector<char>inter(64);
	for (auto& m : motion) {
		fread(&name[0], sizeof(char) * 15, 1, fp);	//ボーン名
		fread(&m.frameNo, sizeof(m.frameNo)				//フレーム番号
			/*+ sizeof(m.location)*/						//位置（IKの時に使用）
			/*+ sizeof(m.quaternion)*/						//クォータニオン
			/*+ sizeof(m.interpolation)*/, 1, fp);			//補間ベジェデータ
		fread(&m.location, sizeof(m.location), 1, fp);
		fread(&m.quaternion, sizeof(m.quaternion), 1, fp);
		fread(&inter[0], sizeof(char) * 64, 1, fp);


		animation[name].emplace_back(MotionData(m.frameNo, m.location, m.quaternion, inter[48], inter[52], inter[56], inter[60]));
	}

	//キーフレームのソート
	unsigned int d = 0;
	for (auto& m : animation) {
		std::sort(m.second.begin(), m.second.end(), [](const MotionData& a, const MotionData& b) {
			return a.frameNo < b.frameNo; 
		});

		duration += max(m.second.back().frameNo, d);
	}

	fclose(fp);

	return 0;
}

const std::map<std::string, std::vector<MotionData>>& VMDMotion::GetAnimationData() const{
	return animation;
}


VMDMotion::~VMDMotion() {
}
