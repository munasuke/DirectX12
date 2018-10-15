#include "Result.h"
#include "Descriptor.h"



Descriptor::Descriptor() : descriptorHeap(nullptr)
{
}

void Descriptor::InitDescriptor(ID3D12Device* _dev) {
	//�f�X�N���v�^�q�[�v�̍쐬
	descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	descriptorHeapDesc.NumDescriptors = 2;//�\��ʂƗ���ʂ�2��
	descriptorHeapDesc.NodeMask = 0;
	result = _dev->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap));

	//�f�X�N���v�^�n���h���̍쐬
	descriptorHandle = descriptorHeap->GetCPUDescriptorHandleForHeapStart();

	//�T�C�Y���擾
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
