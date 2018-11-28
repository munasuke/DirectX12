#pragma once
#include "PrimitiveObject.h"
class Cone :
	public PrimitiveObject {
public:
	/**
	* @brief �����t���R���X�g���N�^
	* @param dev	�f�o�C�X
	* @param div	������
	* @param r		���a
	*/
	Cone(ID3D12Device* dev, const unsigned int div, const float r);

	void Draw(ID3D12GraphicsCommandList* list);
	ID3D12Resource* GetVBuffer();

	~Cone();
private:
};

