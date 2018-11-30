#pragma once
#include "PrimitiveObject.h"

class Plane :
	public PrimitiveObject {
public:
	/**
	* @brief �����t���R���X�g���N�^
	* @param dev	�f�o�C�X
	* @param pos	���W
	* @param width	��
	* @param depth	���s
	* @param color	�F
	* @details	�����Ŏ󂯎�����p�����[�^�����ƂɃo�b�t�@�ƃr���[�𐶐�����
	*/
	Plane(ID3D12Device* dev, const XMFLOAT3& pos, const float width, const float depth, const XMFLOAT4& color);

	void Draw(ID3D12GraphicsCommandList* list);
	ID3D12Resource* GetVBuffer();

	~Plane();
private:
};

