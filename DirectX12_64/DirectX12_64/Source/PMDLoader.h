#pragma once
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
	DirectX::XMFLOAT3	diffuse;			//拡散反射色
	FLOAT				alpha;				//アルファ色
	FLOAT				specularPower;		//スぺキュラの強さ
	DirectX::XMFLOAT3	specular;			//スペキュラ
	DirectX::XMFLOAT3	ambient;			//アンビエント
	UCHAR				toonIndex;			//トゥーンのインデックス
	UCHAR				edgeFlag;			//輪郭線フラグ
	UINT				vertexCount;		//インデックス数
	CHAR				textureFilePath[20];//テクスチャがあるときのテクスチャパス
};
#pragma pack()

//PMD読み込みクラス
class PMDLoader {
public:
	PMDLoader();

	int Load(const char* _path);
	PMDHeader GetPMDHeader();
	std::vector<PMDVertex> GetPMDVertex();
	std::vector<USHORT> GetIndices();		//インデックス情報を返す

	~PMDLoader();
private:
	PMDHeader pmdHeader;
	std::vector<PMDVertex> vertices;
	UINT indicesNum;
	std::vector<USHORT> indices;
};

