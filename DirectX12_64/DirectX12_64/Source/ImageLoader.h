#pragma once
#include <map>
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
	int Load(const std::wstring path);
	DirectX::TexMetadata GetMetaData();
	uint8_t* GetScratchImage();
	ImageRect GetImageRect();
	~ImageLoader();
private:
	DirectX::TexMetadata metaData;
	DirectX::ScratchImage image;
	std::map<std::wstring,
		std::function<HRESULT(const std::wstring& path, DirectX::TexMetadata* meta, DirectX::ScratchImage& img)>
	> loadFuncTbl;
	ImageRect imageRec;
};

