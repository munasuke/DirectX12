#pragma once
#include <tchar.h>
#include <vector>
#include <DirectXTex/DirectXTex.h>
#pragma comment(lib, "DirectXTex.lib")

class TextureResource : public Result {
public:
	TextureResource();

	//void Initialize(ID3D12Device* _dev, const UINT textureNum, const UINT sizeWidth, const UINT sizeHeight);
	void Initialize(ID3D12Device* _dev, DirectX::TexMetadata metaData);

	void WriteToTextureBuffer(std::vector<CHAR> _data, std::vector<bool> textureFlag);
	void WriteToTextureBuffer(DirectX::TexMetadata metaData, uint8_t* img, std::vector<bool> textureFlag);

	ID3D12Resource* GetTextureBuffer();

	~TextureResource();
private:
	D3D12_RESOURCE_DESC texResourceDesc = {};
	D3D12_HEAP_PROPERTIES hprop = {};
	ID3D12Resource* textureBuffer;
	D3D12_BOX box = {};
};
