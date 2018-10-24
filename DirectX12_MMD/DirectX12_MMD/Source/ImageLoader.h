#pragma once
#include "d3dx12.h"
#include <map>
#include <vector>
#include <string>
#include <functional>
#include <memory>
#include <DirectXMath.h>
#include <DirectXTex/DirectXTex.h>
#pragma comment(lib, "DirectXTex.lib")

class Device;

class ImageLoader {
public:
	ImageLoader(std::shared_ptr<Device> dev);

	int Load(const std::string path);
	DirectX::TexMetadata GetMetaData();
	uint8_t* GetScratchImage();
	std::vector<DirectX::XMINT2> GetImageRect();
	std::vector<ID3D12Resource*> GetTextureBuffer();

	~ImageLoader();
private:
	std::wstring ConvertStringToWString(std::string str);

	std::weak_ptr<Device> dev;

	DirectX::TexMetadata	metaData;
	DirectX::ScratchImage	image;

	//Šg’£q‚Åƒ[ƒhŠÖ”‚ğ•ª‚¯‚é
	std::map
		<std::wstring,
		std::function<HRESULT(const std::wstring& path,
			DirectX::TexMetadata* meta,
			DirectX::ScratchImage& img)>> loadFuncTbl;

	std::vector<DirectX::XMINT2> imageRect;
	std::vector<uint8_t*> imageData;

	std::vector<ID3D12Resource*> textureBuffer;
};
