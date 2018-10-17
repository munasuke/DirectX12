#pragma once
#include <map>
#include <vector>
#include <string>
#include <functional>
#include <DirectXTex/DirectXTex.h>
#pragma comment(lib, "DirectXTex.lib")

class ImageLoader {
public:
	ImageLoader();

	int Load(const std::string path);
	DirectX::TexMetadata GetMetaData();
	uint8_t* GetScratchImage();
	~ImageLoader();
private:
	std::wstring ConvertStringToWString(std::string str);

	DirectX::TexMetadata	metaData;
	DirectX::ScratchImage	image;

	//Šg’£q‚Åƒ[ƒhŠÖ”‚ğ•ª‚¯‚é
	std::map
		<std::wstring,
		std::function<HRESULT(const std::wstring& path,
			DirectX::TexMetadata* meta,
			DirectX::ScratchImage& img)>> loadFuncTbl;
};
