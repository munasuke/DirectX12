#include "Result.h"
#include "Descriptor.h"



Descriptor::Descriptor() : descriptorHeap(nullptr)
{
}

void Descriptor::InitDescriptor(ID3D12Device* _dev) {
	//デスクリプタヒープの作成
	descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	descriptorHeapDesc.NumDescriptors = 2;//表画面と裏画面の2つ
	descriptorHeapDesc.NodeMask = 0;
	result = _dev->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap));

	//デスクリプタハンドルの作成
	descriptorHandle = descriptorHeap->GetCPUDescriptorHandleForHeapStart();

	//サイズを取得
	descriptorSize = _dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
}

ID3D12DescriptorHeap* Descriptor::GetDescriptorHeap() {
	return descriptorHeap;
}

CD3DX12_CPU_DESCRIPTOR_HANDLE Descriptor::GetDescriptorHandle()
{
	return descriptorHandle;
}

UINT Descriptor::GetDescriptorSize() {
	return descriptorSize;
}


Descriptor::~Descriptor() {
	ReleaseP(descriptorHeap);
}
