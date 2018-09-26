#include "Result.h"
#include "ConstantBuffer.h"
#include "Window.h"
#include <iostream>

using namespace DirectX;

ConstantBuffer::ConstantBuffer() :
	constantBuffer(nullptr), cbvDescHeap(nullptr), data(nullptr)
{
	mt = {};
}

void ConstantBuffer::Initialize(ID3D12Device * _dev, ID3D12DescriptorHeap* _heap) {
	//�����A�����_�A��x�N�g��
	XMVECTOR eye	= { 0.0f, 10.0f, -15.0f };
	XMVECTOR focus	= { 0.0f, 10.0f,   0.0f };
	XMVECTOR upper	= { 0.0f,  1.0f,   0.0f };

	//���[���h�r���[�v���W�F�N�V����
	mt.world		= XMMatrixIdentity();
	mt.view			= XMMatrixLookAtLH(eye, focus, upper);
	mt.projection	= XMMatrixPerspectiveFovLH(90.0f * 3.14159264f / 180.0f, 
		static_cast<FLOAT>(WIN_WIDTH) / static_cast<FLOAT>(WIN_HEIGHT), 0.01f, 500.0f);

	//�f�X�N���v�^�q�[�v�̍쐬
	//cbvHeapDesc.Type			= D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;	//�R���X�^���g�o�b�t�@
	//cbvHeapDesc.NumDescriptors	= 1;
	//cbvHeapDesc.Flags			= D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;							//�V�F�[�_���猩����悤�ɂ���
	//cbvHeapDesc.NodeMask		= 0;
	//result = _dev->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&cbvDescHeap));

	heapProperties.Type					= D3D12_HEAP_TYPE_UPLOAD;
	heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapProperties.CPUPageProperty		= D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProperties.VisibleNodeMask		= 1;
	heapProperties.CreationNodeMask		= 1;

	//���\�[�X�ݒ�
	cbvResDesc.Dimension		= D3D12_RESOURCE_DIMENSION_BUFFER;	//1�����o�b�t�@
	cbvResDesc.Width			= (sizeof(mt) + 0xff) &~ 0xff;		//255�A���C�����g
	cbvResDesc.Height			= 1;								//1�����Ȃ̂łP��ݒ�
	cbvResDesc.DepthOrArraySize = 1;								//�[���͂Ȃ��̂łP��ݒ�
	cbvResDesc.MipLevels		= 1;								//�~�b�v�͂Ȃ�
	cbvResDesc.SampleDesc.Count = 1;								//�Ȃ��Ǝ��s����
	cbvResDesc.Layout			= D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	result = _dev->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&cbvResDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constantBuffer));

	//�R���X�^���g�o�b�t�@�r���[�̐ݒ�
	cbvDesc.BufferLocation	= constantBuffer->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes		= (sizeof(mt) + 0xff) &~ 0xff;

	//�V�F�[�_���\�[�X�̃q�[�v�̐擪���󂯎��
	auto handle = _heap->GetCPUDescriptorHandleForHeapStart();
	//�|�C���^���T�C�Y���i�߂�
	handle.ptr += _dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	//�i�߂����CreateBufferView�ɓn��
	//�萔�o�b�t�@�̍쐬
	_dev->CreateConstantBufferView(&cbvDesc, handle);

	//�V�F�[�_�ɍs���n��
	D3D12_RANGE range = {0,sizeof(mt )};
	result = constantBuffer->Map(0, &range, (void**)(&data));
	memcpy(data, &mt, sizeof(mt));
}


ConstantBuffer::~ConstantBuffer() {
	constantBuffer->Unmap(0, nullptr);
	ReleaseP(constantBuffer);
	ReleaseP(cbvDescHeap);
}

void ConstantBuffer::UpDataWVP(void) {
	//���f���̉�]
	static FLOAT angle = 0.0f;
	mt.world = XMMatrixRotationY(angle * 3.14159264f / 180.0f);
	memcpy(data, &mt, sizeof(mt));
	++angle;
	
	//std::cout << angle * 3.14159264f / 180.0f << std::endl;
}

void ConstantBuffer::SetDescriptor(ID3D12GraphicsCommandList * _list, int _index, ID3D12DescriptorHeap* _heap, ID3D12Device* _dev) {
	auto handle = _heap->GetGPUDescriptorHandleForHeapStart();
	handle.ptr += _index * _dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	
	_list->SetDescriptorHeaps(1, &_heap);
	_list->SetGraphicsRootDescriptorTable(_index, handle);
}
