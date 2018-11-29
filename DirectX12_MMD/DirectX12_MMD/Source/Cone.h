#pragma once
#include "PrimitiveObject.h"
#include <vector>

class Cone :
	public PrimitiveObject {
public:
	/**
	* @brief �����t���R���X�g���N�^
	* @param dev	�f�o�C�X
	* @param div	������
	* @param r		���a
	* @param height	����
	*/
	Cone(ID3D12Device* dev, const XMFLOAT3& pos, const unsigned int div, const float r, const float height, const XMFLOAT4& color);

	void Draw(ID3D12GraphicsCommandList* list);
	ID3D12Resource* GetVBuffer();

	~Cone();
private:
	std::vector<PrimitiveVertex> vertices;
};

