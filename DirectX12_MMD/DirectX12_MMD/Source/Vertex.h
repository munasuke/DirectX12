#pragma once

#include "PMDLoader.h"
#include <d3d12.h>
#include <DirectXMath.h>
#include <vector>

//頂点構造体
struct VERTEX {
	DirectX::XMFLOAT3 pos;	//座標
	DirectX::XMFLOAT2 uv;	//uv座標
};

class Vertex : public Result {
public:
	Vertex();
	void Initialize(ID3D12Device* _dev, std::vector<PMDVertex> _pmdV);//初期化
	D3D12_INPUT_ELEMENT_DESC* GetInputDesc();
	D3D12_INPUT_ELEMENT_DESC* GetPeraInputDesc();
	UINT GetInputDescNum();
	UINT GetPeraInputDescNum();
	D3D12_VERTEX_BUFFER_VIEW GetVBV();
	D3D12_VERTEX_BUFFER_VIEW GetPeraVBV();
	~Vertex();
private:
	ID3D12Resource* vertexBuffer;//頂点バッファ
	UCHAR* pData;
	D3D12_VERTEX_BUFFER_VIEW vbView = {};

	D3D12_VERTEX_BUFFER_VIEW peraVBV = {};
};

