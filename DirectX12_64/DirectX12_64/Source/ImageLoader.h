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

	//‰æ‘œƒtƒ@ƒCƒ‹‚Ì“Ç‚İ‚İ
	int Load(const std::string path);
	DirectX::TexMetadata GetMetaData();
	std::vector<DirectX::TexMetadata> GetMetaArray();
	uint8_t* GetScratchImage();
	ImageRect GetImageRect();
	~ImageLoader();
private:
	//string‚ğwstring‚É•ÏŠ·
	std::wstring ConvertStringToWString(std::string str);

	DirectX::TexMetadata metaData;
	std::vector<DirectX::TexMetadata> metaDataArray;
	DirectX::ScratchImage image;
	std::map
		<std::wstring, 
		std::function<HRESULT(const std::wstring& path, 
			DirectX::TexMetadata* meta, 
			DirectX::ScratchImage& img)>> loadFuncTbl;
	ImageRect imageRec;
};

