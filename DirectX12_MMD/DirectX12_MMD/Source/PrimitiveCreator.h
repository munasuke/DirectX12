#pragma once
#include <memory>
#include <vector>
#include <DirectXMath.h>
#include "d3dx12.h"

class PrimitiveObject;
class Plane;
class Cone;

using namespace DirectX;

class PrimitiveManager {
public:
	PrimitiveManager(ID3D12Device* d);

	//プリミティブ用のレイアウト、シェーダ、パイプラインの初期化
	void Initialize();

	//プリミティブ用のパイプラインのセット。描画前に必ず呼ぶ
	void SetPrimitiveDrawMode(ID3D12GraphicsCommandList* list);

	//描画
	void Draw(ID3D12GraphicsCommandList* list);

	void SetVertices(ID3D12GraphicsCommandList* list);

	/**
	* @brief 平面オブジェクトの生成
	* @param pos	平面の座標
	* @param width	平面の幅
	* @param depth	平面の奥行
	* @param color	平面の色
	*/
	std::shared_ptr<Plane> CreatePlane(const XMFLOAT3& pos, const float width, const float depth, const XMFLOAT4& color);

	/**
	* @brief 円錐オブジェクトの生成
	* @param pos	円錐の座標
	* @param div	円錐の分割数
	* @param r		円錐の半径
	* @param height	円錐の高さ
	* @param color	円錐の色
	*/
	std::shared_ptr<Cone> CreateCone(const XMFLOAT3& pos, const unsigned int div, const float r, const float height, const XMFLOAT4& color);

	~PrimitiveManager();

private:
	ID3D12PipelineState* piplineState;
	ID3D12RootSignature* rs;
	ID3D12Resource* resource;
	std::vector<std::shared_ptr<PrimitiveObject>> obj;
	ID3D12Device* dev;
};

