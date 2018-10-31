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
	std::vector<char>inter(64);
	for (auto& m : motion) {
		fread(&name[0], sizeof(char) * 15, 1, fp);			//�{�[����
		fread(&m.frameNo, sizeof(m.frameNo), 1, fp);		//�t���[���ԍ�
		fread(&m.location, sizeof(m.location), 1, fp);		//�ʒu�iIK�̎��Ɏg�p�j
		fread(&m.quaternion, sizeof(m.quaternion), 1, fp);	//�N�H�[�^�j�I��
		fread(&inter[0], sizeof(char) * 64, 1, fp);			//��ԃx�W�F�f�[�^

		//�}�b�v�ŊǗ�
		animation[name].emplace_back(
			MotionData(m.frameNo, m.location, m.quaternion,  inter[48], inter[52], inter[56], inter[60])
		);
	}

	//�L�[�t���[���̃\�[�g
	unsigned int d = 0;
	duration = 0;
	for (auto& m : animation) {
		std::sort(m.second.begin(), m.second.end(), [](const MotionData& a, const MotionData& b) {
			return a.frameNo < b.frameNo;
		});
		
		//�����Ԃ��擾
		duration += max(m.second.back().frameNo, d);
	}

	fclose(fp);

	return 0;
}

const std::map<std::string, std::vector<MotionData>>& VMDMotion::GetAnimationData() const{
	return animation;
}

unsigned int VMDMotion::GetDuration() const {
	return duration;
}


VMDMotion::~VMDMotion() {
}
