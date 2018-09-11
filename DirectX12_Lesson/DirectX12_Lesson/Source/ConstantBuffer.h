#pragma once
#include <memory>

class Matrix;

//定数バッファ
class ConstantBuffer : public Result{
public:
	ConstantBuffer();
	void Initialize(ID3D12Device* _dev);
	~ConstantBuffer();
private:
	ID3D12Resource* constantBuffer;
	ID3D12DescriptorHeap* cbvDescHeap;

	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc = {};
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	CD3DX12_HEAP_PROPERTIES heapProperties = {};


	std::shared_ptr<Matrix> matrix;
};

