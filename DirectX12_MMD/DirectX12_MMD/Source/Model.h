#pragma once
#include "d3dx12.h"
#include <DirectXMath.h>
#include <DirectXTex/DirectXTex.h>
#include <memory>
#include <vector>

#pragma comment(lib, "DirectXTex.lib")

//PMD
class PMDLoader;
struct PMDMaterial;
struct BoneNode;

//Texture
class ImageLoader;

//VMD
class VMDMotion;
struct MotionData;

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
	Model(std::shared_ptr<PMDLoader> pmd, std::shared_ptr<ImageLoader> img, std::shared_ptr<VMDMotion> vmd);

	//初期化
	void Initialize(ID3D12Device * _dev);
	//更新
	void Update();
	//描画
	void Draw(ID3D12GraphicsCommandList * _list, ID3D12Device * _dev);

	//ボーンバッファ生成
	void CreateBoneBuffer(ID3D12Device* dev);

	//ボーンの再帰関数
	void RecursiveMatrixMultiply(BoneNode* node, DirectX::XMMATRIX& mat);

	//ボーンの回転行列
	void RotationBone(const std::string str, const DirectX::XMFLOAT4& angle, const DirectX::XMFLOAT4& q2 = DirectX::XMFLOAT4(), float t = 0.0f);
	
	//モーション更新
	void MotionUpdate(int framNo);

	//
	float GetBezierYValueFromXWithNewton(float x, DirectX::XMFLOAT2 a, DirectX::XMFLOAT2 b, unsigned int n = 16);

	//ボーンバッファ返す
	ID3D12Resource* GetBoneBuffer();

	~Model();

private:
	std::weak_ptr<PMDLoader>	pmd;
	std::weak_ptr<ImageLoader>	img;
	std::weak_ptr<VMDMotion>	vmd;

	//マテリアルヒープ
	ID3D12DescriptorHeap* heap;
	//マテリアルバッファ
	std::vector<ID3D12Resource*> materialBuffer;
	//テクスチャバッファ
	std::vector<ID3D12Resource*> textureBuffer;
	//マテリアル
	Materials mat;
	//データ
	UINT8* data;
	//テクスチャフラグ
	std::vector<bool> texFlag;

	//ボーンバッファ
	ID3D12Resource* boneBuffer;
	//ボーン行列のデータ
	DirectX::XMMATRIX* boneMatrixData;
};

