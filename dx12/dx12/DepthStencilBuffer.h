#pragma once

//デプスステンシルバッファ
class DepthStencilBuffer : public Result
{
public:
	DepthStencilBuffer();
	//初期化
	void Initialize(ID3D12Device* _dev, D3D12_DESCRIPTOR_HEAP_DESC _heapDesc);
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
};

