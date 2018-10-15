#pragma once
#include <vector>

//シェーダリソースビュー
class ShaderResourceView : public Result {
public:
	ShaderResourceView();
	void Initialize(ID3D12Device* _dev, ID3D12Resource* _texBuffer, UINT size);
	ID3D12DescriptorHeap* GetTextureHeap();
	D3D12_DESCRIPTOR_HEAP_DESC GetDescriptorHeapDesc();
	ID3D12DescriptorHeap* const* GetTextureHeap2();
	~ShaderResourceView();
private:
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	ID3D12DescriptorHeap* textureHeap;
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
};

