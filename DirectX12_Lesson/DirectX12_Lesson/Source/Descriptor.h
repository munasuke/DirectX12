#pragma once

class Descriptor : public Result {
public:
	Descriptor();

	void InitDescriptor(ID3D12Device* _dev);//èâä˙âª
	ID3D12DescriptorHeap* GetDescriptorHeap();
	CD3DX12_CPU_DESCRIPTOR_HANDLE GetDescriptorHandle();
	UINT GetDescriptorSize();

	~Descriptor();
private:
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc = {};
	ID3D12DescriptorHeap* descriptorHeap;
	CD3DX12_CPU_DESCRIPTOR_HANDLE descriptorHandle;
	UINT descriptorSize;
};

