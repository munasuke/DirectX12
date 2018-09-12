#pragma once
#include "Matrix.h"
#include <memory>

//定数バッファ
class ConstantBuffer : public Result{
public:
	ConstantBuffer();
	void Initialize(ID3D12Device* _dev);
	~ConstantBuffer();

	void UpDataWVP(void);//行列に回転を追加
	void SetDescriptor(ID3D12GraphicsCommandList* list);
private:
	Matrixs mt;//空間行列
	UINT* data;


	ID3D12Resource* constantBuffer;
	ID3D12DescriptorHeap* cbvDescHeap;

	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc = {};
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	CD3DX12_HEAP_PROPERTIES heapProperties = {};

	D3D12_RESOURCE_DESC cbvResDesc = {};
};

