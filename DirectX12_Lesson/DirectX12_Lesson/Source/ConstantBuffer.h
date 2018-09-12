#pragma once
#include "Matrix.h"
#include <memory>

//�萔�o�b�t�@
class ConstantBuffer : public Result{
public:
	ConstantBuffer();
	void Initialize(ID3D12Device* _dev);
	~ConstantBuffer();

	void UpDataWVP(void);//�s��ɉ�]��ǉ�
	void SetDescriptor(ID3D12GraphicsCommandList* list);
private:
	Matrixs mt;//��ԍs��
	UINT* data;


	ID3D12Resource* constantBuffer;
	ID3D12DescriptorHeap* cbvDescHeap;

	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc = {};
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	CD3DX12_HEAP_PROPERTIES heapProperties = {};

	D3D12_RESOURCE_DESC cbvResDesc = {};
};

