#pragma once

#include <d3d12.h>
#include <DirectXMath.h>

//頂点構造体
struct VERTEX {
	DirectX::XMFLOAT3 pos;	//座標
	DirectX::XMFLOAT2 uv;	//uv座標
};

class Vertex : public Result {
public:
	Vertex();
	void Initialize(ID3D12Device* _dev);//初期化
	D3D12_INPUT_ELEMENT_DESC* GetInputDesc();
	UINT GetInputDescNum();
	~Vertex();
private:
	ID3D12Resource* vertexBuffer;//頂点バッファ
	UCHAR* pData;
	D3D12_VERTEX_BUFFER_VIEW vbView = {};
};

