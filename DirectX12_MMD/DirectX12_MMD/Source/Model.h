#pragma once
#include "d3dx12.h"
#include <DirectXMath.h>
#include <DirectXTex/DirectXTex.h>
#include <memory>
#include <vector>

#pragma comment(lib, "DirectXTex.lib")

class PMDLoader;
struct PMDMaterial;

struct Materials {
	DirectX::XMFLOAT4	diffuse;	//減衰色
	DirectX::XMFLOAT4	specular;	//光沢色
	DirectX::XMFLOAT4	ambient;	//環境色
	BOOL				texFlag;	//テクスチャありなしフラグ
};

/*
	ヒープの中身
	0.マテリアル
	1.テクスチャ
	2.乗算テクスチャ
	3.加算テクスチャ
*/
class Model {
public:
	Model();

	void Initialize(ID3D12Device * _dev, DirectX::TexMetadata metaData, std::vector<PMDMaterial> material, UINT textureSize);
	void Draw(ID3D12GraphicsCommandList * _list, ID3D12Device * _dev, std::vector<PMDMaterial> material);
	void WriteToTextureBuffer(DirectX::TexMetadata metaData, uint8_t* img, std::vector<bool> textureFlag);

	//白テクスチャ生成
	void CreateWhiteTexture();
	//黒テクスチャ生成
	void CreateBlackTexture();

	~Model();

private:
	//CBV
	std::vector<ID3D12Resource*> materialBuffer;
	//SRV
	std::vector<ID3D12Resource*> textureBuffer;
	ID3D12Resource* whiteBuffer;
	//CBV, SRV用ヒープ
	ID3D12DescriptorHeap* heap;
	//マテリアル
	Materials mat;
	//データ
	UINT8* data;
	//テクスチャフラグ
	std::vector<bool> texFlag;

};

