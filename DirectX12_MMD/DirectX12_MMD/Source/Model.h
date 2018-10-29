#pragma once
#include "d3dx12.h"
#include <DirectXMath.h>
#include <DirectXTex/DirectXTex.h>
#include <memory>
#include <vector>

#pragma comment(lib, "DirectXTex.lib")

class PMDLoader;
struct PMDMaterial;
class ImageLoader;

struct Materials {
	DirectX::XMFLOAT4	diffuse;	//減衰色
	DirectX::XMFLOAT4	specular;	//光沢色
	DirectX::XMFLOAT4	ambient;	//環境色
	BOOL				texFlag;	//テクスチャありなしフラグ
};

/*
	マテリアルヒープ
	0.マテリアル
	1.テクスチャ
	2.乗算テクスチャ
	3.加算テクスチャ

	ボーンヒープ
	0.ボーン行列
*/
class Model {
public:
	Model(std::shared_ptr<PMDLoader> pmd, std::shared_ptr<ImageLoader> img);

	//初期化
	void Initialize(ID3D12Device * _dev);
	//描画
	void Draw(ID3D12GraphicsCommandList * _list, ID3D12Device * _dev);
	//テクスチャ書き込み
	void WriteToTextureBuffer(std::vector<bool> textureFlag);

	//白テクスチャ生成
	void CreateWhiteTexture();
	//黒テクスチャ生成
	void CreateBlackTexture();

	//ボーンバッファ生成
	void CreateBoneBuffer(ID3D12Device* dev);

	~Model();

private:
	std::weak_ptr<PMDLoader>	pmd;
	std::weak_ptr<ImageLoader>	img;

	//マテリアルヒープ
	ID3D12DescriptorHeap* heap;
	//マテリアルバッファ
	std::vector<ID3D12Resource*> materialBuffer;
	//テクスチャバッファ
	std::vector<ID3D12Resource*> textureBuffer;
	//白テクスチャバッファ
	ID3D12Resource* whiteBuffer;
	//黒テクスチャバッファ
	ID3D12Resource* blackBuffer;
	//マテリアル
	Materials mat;
	//データ
	UINT8* data;
	//テクスチャフラグ
	std::vector<bool> texFlag;

	//ボーンヒープ
	ID3D12DescriptorHeap* boneHeap;
	//ボーンバッファ
	ID3D12Resource* boneBuffer;
	//ボーン行列のデータ
	DirectX::XMMATRIX* boneMatrixData;
};

