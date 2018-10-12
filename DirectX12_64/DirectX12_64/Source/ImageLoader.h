#pragma once
#include <map>
#include <string>
#include <functional>
#include <DirectXTex/DirectXTex.h>
#pragma comment(lib, "DirectXTex.lib")

class ImageLoader {
public:
	ImageLoader();

	//画像ファイルの読み込み
	int Load(const std::wstring path);
	const DirectX::Image* GetImage();
	~ImageLoader();
private:
	const DirectX::Image* image;
	std::map<std::wstring, 
		std::function<HRESULT(const std::wstring& path, DirectX::TexMetadata* meta, DirectX::ScratchImage& img)>
	> loadFuncTbl;
};

