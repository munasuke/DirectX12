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

	//‰æ‘œƒtƒ@ƒCƒ‹‚Ì“Ç‚İ‚İ
<<<<<<< HEAD
	int Load(const std::string path);
	DirectX::TexMetadata GetMetaData();
	uint8_t* GetScratchImage();
	ImageRect GetImageRect();
	~ImageLoader();
private:
	//string‚ğwstring‚É•ÏŠ·
	std::wstring ConvertStringToWString(std::string str);

	DirectX::TexMetadata metaData;
	DirectX::ScratchImage image;
	std::map
		<std::wstring, 
		std::function<HRESULT(const std::wstring& path, 
			DirectX::TexMetadata* meta, 
			DirectX::ScratchImage& img)>> loadFuncTbl;
	ImageRect imageRec;
=======
	int Load(const std::wstring path);
	const DirectX::Image* GetImage();
	~ImageLoader();
private:
	const DirectX::Image* image;
	std::map<std::wstring, 
		std::function<HRESULT(const std::wstring& path, DirectX::TexMetadata* meta, DirectX::ScratchImage& img)>
	> loadFuncTbl;
>>>>>>> parent of 6480300... wicå¯¾å¿œã§ããŸ
};

