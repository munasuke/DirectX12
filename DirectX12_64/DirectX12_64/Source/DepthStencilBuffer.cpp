#include "Result.h"
#include "DepthStencilBuffer.h"
#include "Window.h"

DepthStencilBuffer::DepthStencilBuffer() : depthBuffer(nullptr)
{
}

void DepthStencilBuffer::Initialize(ID3D12Device * _dev, D3D12_DESCRIPTOR_HEAP_DESC _heapDesc)
{
	//�[�x�o�b�t�@�̍쐬
	depthResDesc.Dimension			= D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthResDesc.Width				= WIN_WIDTH;													//��ʂɑ΂��Ďg���o�b�t�@
	depthResDesc.Height				= WIN_HEIGHT;													//��ʂɑ΂��Ďg���o�b�t�@
	depthResDesc.DepthOrArraySize	= 1;
	depthResDesc.Format				= DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT;
	depthResDesc.SampleDesc.Count	= 1;
	depthResDesc.Flags				= D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	depthHeapProp.Type					= D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_DEFAULT;
	depthHeapProp.CPUPageProperty		= D3D12_CPU_PAGE_PROPERTY::D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	depthHeapProp.MemoryPoolPreference	= D3D12_MEMORY_POOL::D3D12_MEMORY_POOL_UNKNOWN;

	//�N���A�o�����[�̍쐬
	depthClearValue.DepthStencil.Depth	= 1.0f;									//�[���ő�l
	depthClearValue.Format				= DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT;	//�g���r���[�ƌ`���𑵂���K�v������

	//���\�[�X�쐬
	result = _dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
		&depthResDesc,
		D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthClearValue,
		IID_PPV_ARGS(&depthBuffer)
	);

	//�[�x�o�b�t�@�r���[�̍쐬
	dsvDesc.Format				= DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT;					//�\���`��
	dsvDesc.ViewDimension		= D3D12_DSV_DIMENSION::D3D12_DSV_DIMENSION_TEXTURE2D;	//�f�v�X�E�X�e���V���ւ̃A�N�Z�X���@
	dsvDesc.Flags				= D3D12_DSV_FLAGS::D3D12_DSV_FLAG_NONE;					//�f�v�X�E�X�e���V����ǂݎ���p�ɂ��邩�̃t���O
	dsvDesc.Texture2D.MipSlice	= 0;													//�~�b�v�}�b�v���x���̃C���f�b�N�X

	//�q�[�v�쐬
	heapDesc.Type			= D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	heapDesc.Flags			= D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	heapDesc.NodeMask		= 0;
	heapDesc.NumDescriptors = 1;
	_dev->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&descriptorHeap));
	_dev->CreateDepthStencilView(depthBuffer, &dsvDesc, descriptorHeap->GetCPUDescriptorHandleForHeapStart());
}

void DepthStencilBuffer::SetDescriptor(ID3D12GraphicsCommandList* _list) {
	_list->ClearDepthStencilView(descriptorHeap->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAGS::D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

ID3D12DescriptorHeap * DepthStencilBuffer::GetHeap() {
	return descriptorHeap;
}


DepthStencilBuffer::~DepthStencilBuffer()
{
}
