#pragma once
#include <map>
#include <vector>
#include <string>
#include <functional>
#include <DirectXTex/DirectXTex.h>
#pragma comment(lib, "DirectXTex.lib")

struct ImageRect {
	UINT64 width;
	UINT64 height;
};

class ImageLoader {
public:
	ImageLoader();

	//画像ファイルの読み込み
	int Load(const std::string path);
	DirectX::TexMetadata GetMetaData();
	uint8_t* GetScratchImage();
	ImageRect GetImageRect();
	~ImageLoader();
private:
	//stringをwstringに変換
	std::wstring ConvertStringToWString(std::string str);

	DirectX::TexMetadata metaData;
	DirectX::ScratchImage image;
	std::map
		<std::wstring, 
		std::function<HRESULT(const std::wstring& path, 
			DirectX::TexMetadata* meta, 
			DirectX::ScratchImage& img)>> loadFuncTbl;
	ImageRect imageRec;
};

