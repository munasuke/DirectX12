#include "Model.h"
#include "PMDLoader.h"

using namespace DirectX;

Model::Model() : textureBuffer(nullptr), resource(nullptr), heap(nullptr){
}

void Model::Initialize(ID3D12Device * _dev, TexMetadata metaData, UINT size, std::vector<PMDMaterial> material) {
	/*
		�}�e���A��
	*/
	D3D12_RESOURCE_DESC resourceDesc = {};
	resourceDesc.Dimension			= D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Flags				= D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_NONE;
	resourceDesc.Format				= DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
	resourceDesc.Layout				= D3D12_TEXTURE_LAYOUT::D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	resourceDesc.Height				= 1;
	resourceDesc.Width				= ((sizeof(DirectX::XMFLOAT3) + 0xff) &~0xff) * material.size();
	resourceDesc.MipLevels			= 1;
	resourceDesc.SampleDesc.Count	= 1;
	resourceDesc.Alignment			= 0;
	resourceDesc.DepthOrArraySize	= 1;

	//�v���p�e�B�ݒ�
	CD3DX12_HEAP_PROPERTIES heapProperties = {};
	heapProperties.Type					= D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD;
	heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL::D3D12_MEMORY_POOL_UNKNOWN;
	heapProperties.CPUPageProperty		= D3D12_CPU_PAGE_PROPERTY::D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProperties.VisibleNodeMask		= 1;
	heapProperties.CreationNodeMask		= 1;

	//���\�[�X����
	auto result = _dev->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&resource)
	);

	//�q�[�v�̐ݒ�
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.NumDescriptors = 1 + size;//�}�e���A�����{�e�N�X�`����?
	heapDesc.Type			= D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	heapDesc.Flags			= D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	//�q�[�v����
	result = _dev->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&heap));

	auto handle		= heap->GetCPUDescriptorHandleForHeapStart();
	auto address	= resource->GetGPUVirtualAddress();

	//CBV�̐ݒ�
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation	= address;
	cbvDesc.SizeInBytes		= ((sizeof(DirectX::XMFLOAT3) + 0xff) &~0xff);

	for (UINT i = 0; i < material.size(); ++i) {
		//CBV����
		_dev->CreateConstantBufferView(&cbvDesc, handle);

		address += cbvDesc.SizeInBytes;
		handle.ptr += _dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}

	result = resource->Map(0, nullptr, (void**)&data);
	for (UINT i = 0; i < material.size(); ++i) {
		//�f�B�t���[�Y����
		mat.diffuse.x = material[i].diffuse.x;
		mat.diffuse.y = material[i].diffuse.y;
		mat.diffuse.z = material[i].diffuse.z;
		mat.diffuse.w = material[i].alpha;

		//�X�y�L��������
		mat.specular.x = material[i].specular.x;
		mat.specular.y = material[i].specular.y;
		mat.specular.z = material[i].specular.z;
		mat.specular.w = material[i].specularPower;

		//�A���r�G���g����
		mat.ambient.x = material[i].ambient.x;
		mat.ambient.y = material[i].ambient.y;
		mat.ambient.z = material[i].ambient.z;

		//�e�N�X�`������Ȃ�����
		mat.texFlag = material[i].textureFilePath[0] != '\0' ? true : false;
		texFlag.emplace_back(mat.texFlag);

		memcpy(data, &mat, sizeof(Material));

		data = (UINT8*)(((sizeof(DirectX::XMFLOAT3) + 0xff) &~0xff) + (CHAR*)(data));
	}

	/*
		�e�N�X�`���o�b�t�@
	*/
	//�e�N�X�`�����\�[�X�̐ݒ�
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

	//�v���p�e�B
	D3D12_HEAP_PROPERTIES hprop = {};
	hprop.CPUPageProperty		= D3D12_CPU_PAGE_PROPERTY::D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	hprop.MemoryPoolPreference	= D3D12_MEMORY_POOL::D3D12_MEMORY_POOL_L0;
	hprop.Type					= D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_CUSTOM;
	hprop.CreationNodeMask		= 1;
	hprop.VisibleNodeMask		= 1;

	//���\�[�X����
	auto result = _dev->CreateCommittedResource(
		&hprop,
		D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
		&texResourceDesc,
		D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&textureBuffer));


	/*
		�V�F�[�_���\�[�X�r���[
	*/
	//SRV�̐ݒ�
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format					= DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension			= D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels		= 1;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;


	//SRV����
	//_dev->CreateShaderResourceView(textureBuffer, &srvDesc, handle);
}

void Model::CreateWhiteTexture() {
}

void Model::CreateBlackTexture() {
}


Model::~Model() {
}
