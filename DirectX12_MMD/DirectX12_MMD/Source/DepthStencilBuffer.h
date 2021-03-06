#pragma once
#include <map>
#include <string>

//デプスステンシルバッファ
class DepthStencilBuffer : public Result
{
public:
	DepthStencilBuffer();
	//初期化
	void Initialize(ID3D12Device* _dev);
	void Initialize2(ID3D12Device* _dev);
	std::map<std::string, ID3D12DescriptorHeap*> GetHeapDSVSRV();
	void SetDescriptor(ID3D12GraphicsCommandList* _list);
	ID3D12DescriptorHeap* GetHeap();

	~DepthStencilBuffer();
private:
	D3D12_RESOURCE_DESC depthResDesc = {};
	D3D12_HEAP_PROPERTIES depthHeapProp = {};
	D3D12_CLEAR_VALUE depthClearValue = {};
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	D3D12_CPU_DESCRIPTOR_HANDLE handle = {};
	ID3D12Resource* depthBuffer;

	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	ID3D12DescriptorHeap* descriptorHeap;

	std::map<std::string, ID3D12DescriptorHeap*> heapForDSVSRV;
	ID3D12Resource* dBuffer;
};

