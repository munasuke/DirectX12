#pragma once
#include "d3dx12.h"
#include <Windows.h>
#include <DirectXMath.h>
#include <vector>


//ヘッダー
#pragma pack(1)
struct PMDHeader{
	CHAR	magic[3];		//PMD
	FLOAT	version;		//バージョン
	CHAR	name[20];		//モデル名
	CHAR	comment[256];	//コメント
	UINT	vertexNum;		//頂点数
};
#pragma pack()

//頂点データ
#pragma pack(1)
struct PMDVertex{
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
struct PMDMaterial{
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

struct MAT{
	DirectX::XMFLOAT3	diffuse;			//減衰色
	BOOL texFlag;
};

//PMD読み込みクラス
class PMDLoader {
public:
	PMDLoader();

	int Load(const char* _path);

	void Initialize(ID3D12Device* _dev);
	void Draw(ID3D12GraphicsCommandList * _list, ID3D12Device * _dev, ID3D12DescriptorHeap* texHeap);

	void SetDescriptor(ID3D12GraphicsCommandList * _list, ID3D12Device* _dev);
	void SetDescriptor(ID3D12GraphicsCommandList * _list, ID3D12Device* _dev, UINT index);
	void SetMaterialColor(UINT index);
	void SetData(UINT8* data);

	PMDHeader					GetPMDHeader();	//ヘッダー情報を返す
	std::vector<PMDVertex>		GetPMDVertex();	//頂点情報を返す
	std::vector<USHORT>			GetIndices();	//インデックス情報を返す
	std::vector<PMDMaterial>	GetMaterial();	//マテリアル情報を返す
	MAT							GetMat();
	std::vector<BOOL>			GetTexFlag();
	UINT8*						GetData();		//データを返す

	void UpdateData();

	~PMDLoader();
private:
	PMDHeader					header;		//ヘッダー
	std::vector<PMDVertex>		vertices;	//頂点情報
	UINT						indicesNum;	//インデックス総数
	std::vector<USHORT>			indices;	//インデックス情報
	UINT						materialNum;//マテリアル総数
	std::vector<PMDMaterial>	material;	//マテリアル情報

	MAT mat;
	std::vector<BOOL> texFlag;

	ID3D12Resource* resource;
	ID3D12DescriptorHeap* descriptorHeap;
	UINT8* data;
	D3D12_RESOURCE_DESC resourceDesc = {};
	D3D12_HEAP_PROPERTIES heapProp = {};
};

