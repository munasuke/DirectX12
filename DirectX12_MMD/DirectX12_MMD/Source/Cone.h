#pragma once
#include "PrimitiveObject.h"
class Cone :
	public PrimitiveObject {
public:
	/**
	* @brief 引数付きコンストラクタ
	* @param dev	デバイス
	* @param div	分割数
	* @param r		半径
	*/
	Cone(ID3D12Device* dev, const unsigned int div, const float r);

	void Draw(ID3D12GraphicsCommandList* list);
	ID3D12Resource* GetVBuffer();

	~Cone();
private:
};

