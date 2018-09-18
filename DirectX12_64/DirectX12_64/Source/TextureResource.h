#pragma once
#include <tchar.h>
#include <vector>

//テクスチャリソース
class TextureResource : public Result {
public:
	TextureResource();
	void Initialize(ID3D12Device* _dev);//初期化
	void WriteToTextureBuffer(std::vector<CHAR> _data);//テクスチャバッファへ書き込む
	ID3D12Resource* GetTextureBuffer();
	~TextureResource();
private:
	D3D12_RESOURCE_DESC texResourceDesc = {};
	D3D12_HEAP_PROPERTIES hprop = {};
	ID3D12Resource* textureBuffer;
	D3D12_BOX box = {};
};

