#include "VMDMotion.h"
#include <iostream>
#include <Windows.h>
#include <algorithm>


VMDMotion::VMDMotion() {
}

int VMDMotion::Load(const char * path) {
	FILE* fp = nullptr;
	if (fopen_s(&fp, path, "rb") != 0) {
		MessageBox(nullptr, TEXT("Can't load VMDFile"), TEXT("Error"), MB_OK);
		return -1;
	}

	//�ŏ���50�o�C�g�͖��ʂȃf�[�^�Ȃ̂Ŕ�΂�
	fseek(fp, 50, SEEK_SET);

	//�L�[�t���[����
	fread(&keyFlameNum, sizeof(keyFlameNum), 1, fp);

	//�L�[�t���[���f�[�^
	std::string name = "";
	name.resize(15);
	motion.resize(keyFlameNum);
	XMFLOAT3 location;
	std::vector<char>inter(64);
	for (auto& m : motion) {
		fread(&name[0], sizeof(char) * 15, 1, fp);	//�{�[����
		fread(&m.frameNo, sizeof(m.frameNo)				//�t���[���ԍ�
			/*+ sizeof(m.location)*/						//�ʒu�iIK�̎��Ɏg�p�j
			/*+ sizeof(m.quaternion)*/						//�N�H�[�^�j�I��
			/*+ sizeof(m.interpolation)*/, 1, fp);			//��ԃx�W�F�f�[�^
		fread(&location, sizeof(XMFLOAT3), 1, fp);
		fread(&m.quaternion, sizeof(m.quaternion), 1, fp);
		fread(&inter[0], sizeof(char) * 64, 1, fp);


		animation[name].emplace_back(MotionData(m.frameNo, m.quaternion));
	}

	//�L�[�t���[���̃\�[�g
	for (auto& m : animation) {
		std::sort(m.second.begin(), m.second.end(), [](const MotionData& a, const MotionData& b) {
			return a.frameNo < b.frameNo; 
		});
	}

	fclose(fp);

	return 0;
}

const std::map<std::string, std::vector<MotionData>>& VMDMotion::GetAnimationData() const{
	return animation;
}


VMDMotion::~VMDMotion() {
}
