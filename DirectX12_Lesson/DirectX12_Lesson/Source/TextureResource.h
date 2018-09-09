#pragma once

//テクスチャリソース
class TextureResource : public Result {
public:
	TextureResource();
	void Initialize(ID3D12Device* _dev);//初期化
	~TextureResource();
private:
	D3D12_RESOURCE_DESC texResourceDesc = {};
	D3D12_HEAP_PROPERTIES hprop = {};
	ID3D12Resource* textureBuffer;
};

