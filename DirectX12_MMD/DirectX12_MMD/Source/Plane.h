#pragma once
#include "PrimitiveObject.h"

class Plane :
	public PrimitiveObject {
public:
	/**
	* @brief 引数付きコンストラクタ
	* @param dev	デバイス
	* @param pos	座標
	* @param width	幅
	* @param depth	奥行
	* @param color	色
	* @details	引数で受け取ったパラメータをもとにバッファとビューを生成する
	*/
	Plane(ID3D12Device* dev, const XMFLOAT3& pos, const float width, const float depth, const XMFLOAT4& color);

	void Draw(ID3D12GraphicsCommandList* list);
	void SetVertices(ID3D12GraphicsCommandList* list);
	ID3D12Resource* GetVBuffer();
	D3D12_VERTEX_BUFFER_VIEW GetVBV();

	~Plane();
private:
};

