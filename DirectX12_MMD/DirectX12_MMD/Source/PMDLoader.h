#pragma once
#include "d3dx12.h"
#include <Windows.h>
#include <DirectXMath.h>
#include <vector>
#include <memory>

class BmpLoader;
class ImageLoader;

//ヘッダー
#pragma pack(1)
struct PMDHeader {
	CHAR	magic[3];		//PMD
	FLOAT	version;		//バージョン
	CHAR	name[20];		//モデル名
	CHAR	comment[256];	//コメント
	UINT	vertexNum;		//頂点数
};
#pragma pack()

//頂点データ
#pragma pack(1)
struct PMDVertex {
	DirectX::XMFLOAT3	pos;	//座標
	DirectX::XMFLOAT3	normal;	//法線
	DirectX::XMFLOAT2	uv;		//uv

	USHORT	bornNum[2];	//ボーン番号
	UCHAR	bornWeight;	//ウエイト
	UCHAR	edgeFlag;	//輪郭線フラグ
};
#pragma pack()

//マテリアルデータ
#pragma pack(1)
struct PMDMaterial {
	DirectX::XMFLOAT3	diffuse;			//減衰色
	FLOAT				alpha;				//減衰色の不透明度
	FLOAT				specularPower;		//光沢色の強さ
	DirectX::XMFLOAT3	specular;			//光沢色
	DirectX::XMFLOAT3	ambient;			//環境色
	UCHAR				toonIndex;			//トゥーンのインデックス
	UCHAR				edgeFlag;			//輪郭線フラグ
	UINT				indexCount;			//面頂点数（面頂点リストのデータ数）
	CHAR				textureFilePath[20];//テクスチャがあるときのテクスチャパス
};
#pragma pack()

//ボーン情報
struct PMDBone {
	CHAR				boneName[20];		//ボーン名
	USHORT				parentBoneIndex;	//親のボーン番号
	USHORT				tailPosoneIndex;	//tail位置のボーン番号
	UCHAR				boneType;			//ボーンの種類
	USHORT				ikParentBoneIndex;	//IKボーン番号
	DirectX::XMFLOAT3	boneHeadPos;		//ボーンのヘッドの位置
};

//GPUにおくるマテリアル情報
struct MAT {
	DirectX::XMFLOAT4	diffuse;	//減衰色
	DirectX::XMFLOAT4	specular;	//光沢色
	DirectX::XMFLOAT4	ambient;	//環境色
	BOOL				texFlag;	//テクスチャありなしフラグ
};

//PMD読み込みクラス
class PMDLoader {
public:
	PMDLoader(std::shared_ptr<BmpLoader> bmp, std::shared_ptr<ImageLoader> imageL);

	int Load(const char* _path);

	void SetData(UINT8* data);

	PMDHeader					GetPMDHeader();	//ヘッダー情報を返す
	std::vector<PMDVertex>		GetPMDVertex();	//頂点情報を返す
	std::vector<USHORT>			GetIndices();	//インデックス情報を返す
	std::vector<PMDMaterial>	GetMaterial();	//マテリアル情報を返す
	MAT							GetMat();
	std::vector<bool>			GetTexFlag();
	/*モデルパスとテクスチャ相対パスから、アプリからテクスチャのパスを生成する。
	//モデルの相対パス（絶対パスでも可）
	//テクスチャの相対パス（相対パスオンリー）
	*/
	std::string					GetRelativeTexturePathFromPmdPath(const char* modelPath, const char* texturePath);
	UINT8*						GetData();		//データを返す
	UINT						GetTextureNum();

	void UpdateData();

	~PMDLoader();
private:
	std::weak_ptr<BmpLoader>	bmp;		//BMP
	std::weak_ptr<ImageLoader>	imageL;

	UINT textureNum;

	PMDHeader					header;		//ヘッダー
	std::vector<PMDVertex>		vertices;	//頂点情報
	UINT						indicesNum;	//インデックス総数
	std::vector<USHORT>			indices;	//インデックス情報
	UINT						materialNum;//マテリアル総数
	std::vector<PMDMaterial>	material;	//マテリアル情報

	MAT mat;
	std::vector<bool> texFlag;

	ID3D12Resource* resource;
	ID3D12DescriptorHeap* descriptorHeap;
	UINT8* data;
	D3D12_RESOURCE_DESC resourceDesc = {};
	D3D12_HEAP_PROPERTIES heapProp = {};
};

