#pragma once
#include "PrimitiveObject.h"
#include <vector>

class Cone :
	public PrimitiveObject {
public:
	/**
	* @brief 引数付きコンストラクタ
	* @param dev	デバイス
	* @param pos	座標
	* @param div	分割数
	* @param r		半径
	* @param height	高さ
	* @param color	色
	* @details	引数で受け取ったパラメータをもとにバッファとビューを生成する
	*/
	Cone(ID3D12Device* dev, const XMFLOAT3& pos, const unsigned int div, const float r, const float height, const XMFLOAT4& color);

	/**
	* @brief 描画処理
	* @param list	コマンドリスト
	* @return なし
	*/
	void Draw(ID3D12GraphicsCommandList* list);
	ID3D12Resource* GetVBuffer();

	~Cone();
private:
	std::vector<PrimitiveVertex> vertices;
};

