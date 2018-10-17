#include "Model.h"
#include "Window.h"

using namespace DirectX;

Model::Model() : textureBuffer(nullptr), resource(nullptr), heap(nullptr){
}

void Model::Initialize(ID3D12Device * _dev, TexMetadata metaData, UINT size, UINT materialSize) {
	/*
		�e�N�X�`���o�b�t�@
	*/
	D3D12_RESOURCE_DESC texResourceDesc = {};
	CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	texResourceDesc.Dimension			= D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	texResourceDesc.Width				= metaData.width;
	texResourceDesc.Height				= metaData.height;
	texResourceDesc.DepthOrArraySize	= 1;
	texResourceDesc.Format				= DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
	texResourceDesc.SampleDesc.Count	= 1;
	texResourceDesc.Flags				= D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_NONE;
	texResourceDesc.Layout				= D3D12_TEXTURE_LAYOUT::D3D12_TEXTURE_LAYOUT_UNKNOWN;

	D3D12_HEAP_PROPERTIES hprop = {};
	hprop.CPUPageProperty		= D3D12_CPU_PAGE_PROPERTY::D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	hprop.MemoryPoolPreference	= D3D12_MEMORY_POOL::D3D12_MEMORY_POOL_L0;
	hprop.Type					= D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_CUSTOM;
	hprop.CreationNodeMask		= 1;
	hprop.VisibleNodeMask		= 1;

	auto result = _dev->CreateCommittedResource(
		&hprop,
		D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
		&texResourceDesc,
		D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&textureBuffer));

	/*
		�q�[�v�̐���
	*/
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.NumDescriptors = 2 + size;//�̈�m��
	heapDesc.Type			= D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	heapDesc.Flags			= D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	result = _dev->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&heap));

	//�v���p�e�B�ݒ�
	CD3DX12_HEAP_PROPERTIES heapProperties = {};
	heapProperties.Type					= D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD;
	heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL::D3D12_MEMORY_POOL_UNKNOWN;
	heapProperties.CPUPageProperty		= D3D12_CPU_PAGE_PROPERTY::D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProperties.VisibleNodeMask		= 1;
	heapProperties.CreationNodeMask		= 1;


	/*
		�萔�o�b�t�@
	*/
	//�����A�����_�A��x�N�g��
	XMFLOAT3 eye  (0.0f, 10.0f, -15.0f);
	XMFLOAT3 focus(0.0f, 10.0f,   0.0f);
	XMFLOAT3 upper(0.0f,  1.0f,   0.0f);

	//���[���h�E�r���[�E�v���W�F�N�V�����s��̐���
	matrix.world = XMMatrixIdentity();
	matrix.view = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&focus), XMLoadFloat3(&upper));
	matrix.projection = XMMatrixPerspectiveFovLH(90.0f * 3.14159264f / 180.0f,
		static_cast<FLOAT>(WIN_WIDTH) / static_cast<FLOAT>(WIN_HEIGHT), 0.01f, 500.0f);

	//���\�[�X�ݒ�
	D3D12_RESOURCE_DESC cbvResDesc = {};
	cbvResDesc.Dimension		= D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_BUFFER;//1�����o�b�t�@
	cbvResDesc.Width			= ((sizeof(matrix) + 0xff) &~0xff) * materialSize;			//255�A���C�����g
	cbvResDesc.Height			= 1;														//1�����Ȃ̂łP��ݒ�
	cbvResDesc.DepthOrArraySize = 1;														//�[���͂Ȃ��̂łP��ݒ�
	cbvResDesc.MipLevels		= 1;														//�~�b�v�͂Ȃ�
	cbvResDesc.SampleDesc.Count = 1;														//�Ȃ��Ǝ��s����
	cbvResDesc.Layout			= D3D12_TEXTURE_LAYOUT::D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//���\�[�X����
	result = _dev->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
		&cbvResDesc,
		D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&resource)
	);

	//CBV�̐ݒ�
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = resource->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = (sizeof(matrix) + 0xff) &~0xff;

	//CBV����
	_dev->CreateConstantBufferView(&cbvDesc, heap->GetCPUDescriptorHandleForHeapStart());


	/*
		�}�e���A��
	*/
	for (UINT i = 0; i < materialSize; ++i) { 

	}

	/*
		�V�F�[�_���\�[�X�r���[
	*/
	//SRV�̐ݒ�
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format					= DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension			= D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels		= 1;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	//CBV�̃q�[�v�̐擪���󂯎��A�T�C�Y�����炷
	auto handle = heap->GetCPUDescriptorHandleForHeapStart();
	handle.ptr += _dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	//SRV����
	_dev->CreateShaderResourceView(textureBuffer, &srvDesc, handle);
}

void Model::CreateWhiteTexture() {
}

void Model::CreateBlackTexture() {
}


Model::~Model() {
}
