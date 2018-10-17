#pragma once
#include "d3dx12.h"
#include <DirectXMath.h>
#include <DirectXTex/DirectXTex.h>
#include <memory>

#pragma comment(lib, "DirectXTex.lib")

class Window;

//空間行列
struct Matrixs {
	DirectX::XMMATRIX world;		//ワールド行列
	DirectX::XMMATRIX view;			//ビュー行列
	DirectX::XMMATRIX projection;	//プロジェクション行列
};

/*
	マテリアル
	テクスチャ
	乗算テクスチャ
	加算テクスチャ
*/
class Model {
public:
	Model();

	void Initialize(ID3D12Device * _dev, DirectX::TexMetadata metaData, UINT size, UINT materialSize);

	void CreateWhiteTexture();
	void CreateBlackTexture();

	~Model();

private:
	//テクスチャバッファ
	ID3D12Resource* textureBuffer;
	//CBV
	ID3D12Resource* resource;
	//CBV, SRV用ヒープ
	ID3D12DescriptorHeap* heap;
	//行列
	Matrixs matrix;

};

