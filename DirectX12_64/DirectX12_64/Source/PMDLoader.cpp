#include "PMDLoader.h"
#include "BmpLoader.h"
#include "ImageLoader.h"
#include <tchar.h>
#include <stdio.h>
#include <iostream>
#include <string>

PMDLoader::PMDLoader(std::shared_ptr<BmpLoader> bmp, std::shared_ptr<ImageLoader> imageL) :
	textureNum(0),
	resource(nullptr),
	descriptorHeap(nullptr),
	data(nullptr)
{
	this->bmp = bmp;
	this->imageL = imageL;
	mat = {};
}

int PMDLoader::Load(const char * _path) {
	FILE* fp = nullptr;
	if (fopen_s(&fp, _path, "rb") != 0){
		MessageBox(nullptr, TEXT("Can't load PMDFile"), TEXT("Error"), MB_OK);
		return -1;
	}
	//�w�b�_�[�ǂݍ���
	fread(&header, sizeof(header), 1, fp);
	//fread(&pmd.header.magic, sizeof(pmd.header.magic), 1, fp);
	//fread(&pmd.header.version, sizeof(pmd.header) - sizeof(pmd.header.magic), 1, fp);

	//���_�f�[�^�ǂݍ���
	vertices.resize(header.vertexNum);
	fread(&vertices[0], sizeof(PMDVertex), header.vertexNum, fp);

	//�C���f�b�N�X�����ǂݍ���
	fread(&indicesNum, sizeof(UINT), 1, fp);
	//�C���f�b�N�X�f�[�^�ǂݍ���
	indices.resize(indicesNum);
	fread(&indices[0], sizeof(USHORT), indicesNum, fp);

	//�}�e���A�������ǂݍ���
	fread(&materialNum, sizeof(UINT), 1, fp);
	//�}�e���A���f�[�^�ǂݍ���
	material.resize(materialNum);
	fread(&material[0], sizeof(PMDMaterial), materialNum, fp);

	fclose(fp);

	//�e�N�X�`���ǂݍ���
	for (INT i = 0; i < material.size(); ++i){
		//�e�N�X�`���������ꍇ�̂ݓǂݍ���
		if (strlen(material[i].textureFilePath) > 0) {
			//�e�N�X�`���̖���
			++textureNum;
			//���΃p�X���擾
			auto texPath = GetRelativeTexturePathFromPmdPath(_path, material[i].textureFilePath);
			//�e�N�X�`���ǂݍ���
			imageL.lock()->Load(texPath.c_str());
		}
	}

	return 0;
}

void PMDLoader::Initialize(ID3D12Device * _dev) {
	heapProp.Type					= D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD;
	heapProp.CPUPageProperty		= D3D12_CPU_PAGE_PROPERTY::D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProp.MemoryPoolPreference	= D3D12_MEMORY_POOL::D3D12_MEMORY_POOL_UNKNOWN;
	heapProp.CreationNodeMask		= 1;
	heapProp.VisibleNodeMask		= 1;

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

	auto result = _dev->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&resource)
	);


	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc	= {};
	cbvHeapDesc.NumDescriptors				= material.size();
	cbvHeapDesc.Flags						= D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	cbvHeapDesc.NodeMask					= 0;
	cbvHeapDesc.Type						= D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	_dev->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&descriptorHeap));

	//�}�e���A����CBV���쐬����
	auto handle		= descriptorHeap->GetCPUDescriptorHandleForHeapStart();
	auto address	= resource->GetGPUVirtualAddress();
	for(UINT i = 0; i < material.size(); ++i) {
		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
		cbvDesc.BufferLocation	= address;
		cbvDesc.SizeInBytes		= ((sizeof(DirectX::XMFLOAT3) + 0xff) &~0xff);
		_dev->CreateConstantBufferView(&cbvDesc, handle);

		address += cbvDesc.SizeInBytes;
		handle.ptr += _dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}

	//�}�e���A�����V�F�[�_�ɓn��
	result = resource->Map(0, nullptr, (void**)(&data));
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

		//�e�N�X�`���̂����Ȃ�����
		mat.texFlag = material[i].textureFilePath[0] != '\0' ? true : false;
		texFlag.emplace_back(mat.texFlag);

		//�}�e���A���ԍ��ƃe�N�X�`���̂����Ȃ����\��
		std::cout << i << ":" << std::boolalpha << (bool)(mat.texFlag) << std::endl;

		//�}�e���A��������GPU�ɓ�����
		memcpy(data, &mat, sizeof(MAT));

		//�f�[�^�����炷
		data = (UINT8*)(((sizeof(DirectX::XMFLOAT3) + 0xff) &~ 0xff) + (CHAR*)(data));
	}
}

void PMDLoader::Draw(ID3D12GraphicsCommandList * _list, ID3D12Device * _dev, ID3D12DescriptorHeap* texHeap) {
	UINT offset = 0;
	for (UINT i = 0; i < material.size(); ++i) {
		//�e�N�X�`���̃f�X�N���v�^���Z�b�g
		_list->SetDescriptorHeaps(1, &texHeap);
		auto texHandle = texHeap->GetGPUDescriptorHandleForHeapStart();
		_list->SetGraphicsRootDescriptorTable(0, texHandle);

		//�}�e���A���̐����f�X�N���v�^���Z�b�g
		auto handle = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
		handle.ptr += i * _dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		_list->SetDescriptorHeaps(1, &descriptorHeap);
		_list->SetGraphicsRootDescriptorTable(2, handle);

		//�}�e���A���ʂɕ`��
		_list->DrawIndexedInstanced(material[i].indexCount, 1, offset, 0, 0);

		//�}�e���A���̃C���f�b�N�X�����炷
		offset += material[i].indexCount;
	}
}

void PMDLoader::SetMaterialColor(UINT index) {
	//�����F
	memcpy(data, &material[index].diffuse, sizeof(DirectX::XMFLOAT3));
}

void PMDLoader::SetData(UINT8 * data) {
	this->data = data;
}

PMDHeader PMDLoader::GetPMDHeader() {
	return header;
}

std::vector<PMDVertex> PMDLoader::GetPMDVertex() {
	return vertices;
}

std::vector<USHORT> PMDLoader::GetIndices() {
	return indices;
}

std::vector<PMDMaterial> PMDLoader::GetMaterial() {
	return material;
}

MAT PMDLoader::GetMat() {
	return mat;
}

std::vector<bool> PMDLoader::GetTexFlag()
{
	return texFlag;
}

std::string PMDLoader::GetRelativeTexturePathFromPmdPath(const char * modelPath, const char * texturePath) {
	//�e�N�X�`���f�[�^�̓ǂݍ���
	//�t�H���_�̋��؂���/��\\�̂ǂ��炩���g������

	std::string strModelPath = modelPath;

	auto index1 = strModelPath.rfind('/');
	auto index2 = strModelPath.rfind('\\');
	index2 = std::string::npos ? 0 : index2;
	//�t�H���_�̋��؂��ɂǂ��炪�g�����Ă��邩���r����
	auto separation = max(index1, index2);
	//�A�v�������̑��΃p�X�𐶐�
	auto textureFilePath = strModelPath.substr(0, separation) + "/" + texturePath;

	return textureFilePath;
}

UINT8 * PMDLoader::GetData(void) {
	return data;
}

UINT PMDLoader::GetTextureNum() {
	return textureNum;
}

void PMDLoader::UpdateData() {
	data = (UINT8*)(((sizeof(DirectX::XMFLOAT3) + 0xff) &~0xff) + (CHAR*)(data));
}

PMDLoader::~PMDLoader() {
}
