#include "Result.h"
#include "ConstantBuffer.h"
#include "Window.h"
#include <iostream>

using namespace DirectX;

ConstantBuffer::ConstantBuffer() :
	cbvDescHeap(nullptr)
{
	data.clear();
	resource.clear();
	mt = {};
}

void ConstantBuffer::Initialize(ID3D12Device * _dev, ID3D12DescriptorHeap* _heap) {
	//�����A�����_�A��x�N�g��
	XMFLOAT3 eye	(0.0f, 10.0f, -15.0f);
	XMFLOAT3 focus	(0.0f, 10.0f,   0.0f);
	XMFLOAT3 upper	(0.0f,  1.0f,   0.0f);

	//���[���h�E�r���[�E�v���W�F�N�V�����s��̍쐬
	mt.world		= XMMatrixIdentity();
	mt.view			= XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&focus), XMLoadFloat3(&upper));
	mt.projection	= XMMatrixPerspectiveFovLH(90.0f * 3.14159264f / 180.0f, 
		static_cast<FLOAT>(WIN_WIDTH) / static_cast<FLOAT>(WIN_HEIGHT), 0.01f, 500.0f);

	//�v���p�e�B�ݒ�
	heapProperties.Type					= D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD;
	heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL::D3D12_MEMORY_POOL_UNKNOWN;
	heapProperties.CPUPageProperty		= D3D12_CPU_PAGE_PROPERTY::D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProperties.VisibleNodeMask		= 1;
	heapProperties.CreationNodeMask		= 1;

	//���\�[�X�ݒ�
	cbvResDesc.Dimension		= D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_BUFFER;//1�����o�b�t�@
	cbvResDesc.Width			= (sizeof(mt) + 0xff) &~ 0xff;								//255�A���C�����g
	cbvResDesc.Height			= 1;														//1�����Ȃ̂łP��ݒ�
	cbvResDesc.DepthOrArraySize = 1;														//�[���͂Ȃ��̂łP��ݒ�
	cbvResDesc.MipLevels		= 1;														//�~�b�v�͂Ȃ�
	cbvResDesc.SampleDesc.Count = 1;														//�Ȃ��Ǝ��s����
	cbvResDesc.Layout			= D3D12_TEXTURE_LAYOUT::D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		//�s�񃊃\�[�X���쐬
	result = _dev->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
		&cbvResDesc,
		D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&resource[name[0]])
	);
	//�R���X�^���g�o�b�t�@�r���[�̐ݒ�
	cbvDesc.BufferLocation	= resource[name[0]]->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes		= (sizeof(mt) + 0xff) &~ 0xff;

	//�V�F�[�_���\�[�X�̃q�[�v�̐擪���󂯎��
	handle = _heap->GetCPUDescriptorHandleForHeapStart();
	//�|�C���^���T�C�Y���i�߂�
	handle.ptr += _dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	//�i�߂����CreateBufferView�ɓn��
	//�萔�o�b�t�@�̍쐬
	_dev->CreateConstantBufferView(&cbvDesc, handle);

	//�V�F�[�_�ɍs���n��
	result = resource[name[0]]->Map(0, nullptr, (void**)(&data[name[0]]));
	memcpy(data[name[0]], &mt, sizeof(mt));
}

void ConstantBuffer::UpDataWVP(void) {
	//���f���̉�]
	static FLOAT angle = 0.0f;
	mt.world = XMMatrixRotationY(angle * 3.14159264f / 180.0f);
	memcpy(data[name[0]], &mt, sizeof(mt));
	++angle;
}

void ConstantBuffer::SetDescriptor(ID3D12GraphicsCommandList * _list, int _index, ID3D12DescriptorHeap* _heap, ID3D12Device* _dev) {
	auto handle = _heap->GetGPUDescriptorHandleForHeapStart();
	handle.ptr += _index * _dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	
	_list->SetDescriptorHeaps(1, &_heap);
	_list->SetGraphicsRootDescriptorTable(_index, handle);
}

void ConstantBuffer::SetMaterial(DirectX::XMFLOAT3 diffuse) {
	memcpy(data[name[1]], &diffuse, sizeof(XMFLOAT3));
}

ID3D12Resource * ConstantBuffer::GetConstantBuffer(int i) {
	return resource[name[i]];
}


D3D12_CPU_DESCRIPTOR_HANDLE ConstantBuffer::GetDescriptorHandle()
{
	return handle;
}

ConstantBuffer::~ConstantBuffer() {
	for(auto itr = resource.begin(); itr != resource.end(); ++itr)
	{
		if(itr->second != nullptr)
		{
			itr->second->Unmap(0, nullptr);
			ReleaseP(itr->second);
		}
	}
	ReleaseP(cbvDescHeap);
}
