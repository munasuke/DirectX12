#pragma once
#include "PrimitiveObject.h"
#include <vector>

class Cone :
	public PrimitiveObject {
public:
	/**
	* @brief �����t���R���X�g���N�^
	* @param dev	�f�o�C�X
	* @param pos	���W
	* @param div	������
	* @param r		���a
	* @param height	����
	* @param color	�F
	* @details	�����Ŏ󂯎�����p�����[�^�����ƂɃo�b�t�@�ƃr���[�𐶐�����
	*/
	Cone(ID3D12Device* dev, const XMFLOAT3& pos, const unsigned int div, const float r, const float height, const XMFLOAT4& color);

	/**
	* @brief �`�揈��
	* @param list	�R�}���h���X�g
	* @return �Ȃ�
	*/
	void Draw(ID3D12GraphicsCommandList* list);
	ID3D12Resource* GetVBuffer();

	~Cone();
private:
	std::vector<PrimitiveVertex> vertices;
};

