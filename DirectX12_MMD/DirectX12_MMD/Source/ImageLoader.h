#pragma once
#include <map>
#include <vector>
#include <string>
#include <functional>
#include <DirectXMath.h>
#include <DirectXTex/DirectXTex.h>
#pragma comment(lib, "DirectXTex.lib")

class ImageLoader {
public:
	ImageLoader();

	int Load(const std::string path);
	DirectX::TexMetadata GetMetaData();
	uint8_t* GetScratchImage();
	std::vector<DirectX::XMINT2> GetImageRect();
	std::vector<uint8_t*> GetImageData();

	~ImageLoader();
private:
	std::wstring ConvertStringToWString(std::string str);

	DirectX::TexMetadata	metaData;
	DirectX::ScratchImage	image;

	//拡張子でロード関数を分ける
	std::map
		<std::wstring,
		std::function<HRESULT(const std::wstring& path,
			DirectX::TexMetadata* meta,
			DirectX::ScratchImage& img)>> loadFuncTbl;

	std::vector<DirectX::XMINT2> imageRect;
	std::vector<uint8_t*> imageData;
};
