#include "Model.h"
#include "PMDLoader.h"
#include "ImageLoader.h"
#include <iostream>

using namespace DirectX;

Model::Model(std::shared_ptr<PMDLoader> pmd, std::shared_ptr<ImageLoader> img) :
	pmd(pmd),
	img(img),
	heap(nullptr), 
	whiteBuffer(nullptr), 
	blackBuffer(nullptr),
	boneHeap(nullptr), 
	boneBuffer(nullptr),
	boneMatrixData(nullptr)
{
}

void Model::Initialize(ID3D12Device * _dev) {
	auto material = pmd.lock()->GetMaterial();
	//ヒープの設定
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.NumDescriptors = material.size() * 2;
	heapDesc.Type			= D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	heapDesc.Flags			= D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	heapDesc.NodeMask		= 0;

	//ヒープ生成
	auto result = _dev->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&heap));

	//マテリアル分のバッファを生成
	auto size = ((sizeof(DirectX::XMFLOAT3) + 0xff) &~0xff);
	materialBuffer.resize(material.size());
	for (UINT i = 0; i < materialBuffer.size(); ++i) {
		auto result = _dev->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(size),
			D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&materialBuffer[i])
		);

		//ディフューズ成分
		mat.diffuse.x = material[i].diffuse.x;
		mat.diffuse.y = material[i].diffuse.y;
		mat.diffuse.z = material[i].diffuse.z;
		mat.diffuse.w = material[i].alpha;

		//スペキュラ成分
		mat.specular.x = material[i].specular.x;
		mat.specular.y = material[i].specular.y;
		mat.specular.z = material[i].specular.z;
		mat.specular.w = material[i].specularPower;

		//アンビエント成分
		mat.ambient.x = material[i].ambient.x;
		mat.ambient.y = material[i].ambient.y;
		mat.ambient.z = material[i].ambient.z;

		mat.texFlag = material[i].textureFilePath[0] != '\0' ? true : false;
		texFlag.emplace_back(mat.texFlag);

		result = materialBuffer[i]->Map(0, nullptr, (void**)&data);

		memcpy(data, &mat, sizeof(Materials));

		materialBuffer[i]->Unmap(0, nullptr);
	}

	//テクスチャリソースの設定
	D3D12_RESOURCE_DESC texResourceDesc = {};
	CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	texResourceDesc.Dimension			= D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	texResourceDesc.Format				= DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
	texResourceDesc.Flags				= D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_NONE;
	texResourceDesc.Layout				= D3D12_TEXTURE_LAYOUT::D3D12_TEXTURE_LAYOUT_UNKNOWN;
	texResourceDesc.DepthOrArraySize	= 1;
	texResourceDesc.SampleDesc.Count	= 1;

	//プロパティ
	D3D12_HEAP_PROPERTIES hprop = {};
	hprop.CPUPageProperty		= D3D12_CPU_PAGE_PROPERTY::D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	hprop.MemoryPoolPreference	= D3D12_MEMORY_POOL::D3D12_MEMORY_POOL_L0;
	hprop.Type					= D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_CUSTOM;
	hprop.CreationNodeMask		= 1;
	hprop.VisibleNodeMask		= 1;

	//白テクスチャバッファの生成
	texResourceDesc.Width	= 4;
	texResourceDesc.Height	= 4 * 4;
	result = _dev->CreateCommittedResource(
		&hprop,
		D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
		&texResourceDesc,
		D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&whiteBuffer));
	result = _dev->CreateCommittedResource(
		&hprop,
		D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
		&texResourceDesc,
		D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&blackBuffer));

	CreateWhiteTexture();
	CreateBlackTexture();

	//SRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format					= DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension			= D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels		= 1;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	//ヒープの開始を表すCPUデスクリプタハンドル
	auto handle = heap->GetCPUDescriptorHandleForHeapStart();

	//次のCPUデスクリプタハンドルまでのサイズ
	auto incrementSize = _dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	//debug
	auto tBuffIndex = 0;
	auto buffLocation = ((sizeof(DirectX::XMFLOAT3) + 0xff) &~0xff);
	//ビュー生成
	for (UINT i = 0; i < materialBuffer.size(); ++i) {
		//CBVの設定
		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
		cbvDesc.SizeInBytes		= buffLocation;
		cbvDesc.BufferLocation	= materialBuffer[i]->GetGPUVirtualAddress();

		//CBV生成
		_dev->CreateConstantBufferView(&cbvDesc, handle);

		//通常テクスチャまでずらす
		handle.ptr += incrementSize;

		//SRV生成
		//マテリアルにテクスチャがあるならテクスチャを、ないなら白テクスチャを生成
		if (texFlag[i]) {
			_dev->CreateShaderResourceView(img.lock()->GetTextureBuffer()[tBuffIndex], &srvDesc, handle);
			++tBuffIndex;
		}
		else {
			_dev->CreateShaderResourceView(whiteBuffer, &srvDesc, handle);
		}

		//マテリアルまでずらす
		handle.ptr += incrementSize;
	}
}

void Model::Draw(ID3D12GraphicsCommandList * _list, ID3D12Device * _dev) {
	auto material = pmd.lock()->GetMaterial();
	//次のマテリアルまでのオフセット
	UINT offset = 0;

	//ヒープの開始を表すGPUデスクリプタハンドル
	auto handle = heap->GetGPUDescriptorHandleForHeapStart();

	//ヒープのセット
	ID3D12DescriptorHeap* heaps[] = { heap };
	_list->SetDescriptorHeaps(1, heaps);

	for (UINT i = 0; i < material.size(); ++i) {
		//マテリアルのセット
		_list->SetGraphicsRootDescriptorTable(1, handle);

		//通常テクスチャまでずらす
		handle.ptr += 2 * _dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		//マテリアル別に描画
		_list->DrawIndexedInstanced(material[i].indexCount, 1, offset, 0, 0);

		//次のマテリアルまでオフセットする
		offset += material[i].indexCount;
	}
}

void Model::WriteToTextureBuffer(std::vector<bool> textureFlag) {
	//UINT index = 0;
	//for (const auto& tFlag : texFlag) {
	//	if (tFlag) {
	//		auto result = textureBuffer[index]->WriteToSubresource(0, nullptr, img.lock()->GetImageData()[index], img.lock()->GetImageRect()[index].x * 4, img.lock()->GetImageRect()[index].y * 4);
	//		++index;
	//	}
	//	else {
	//		CreateWhiteTexture();
	//	}
	//}
}

void Model::CreateWhiteTexture() {
	//白テクスチャデータ
	std::vector<UCHAR> data(4 * 4 * 4);
	//真っ白に染めちゃう
	std::fill(data.begin(), data.end(), 0xff);

	//書き込む
	auto result = whiteBuffer->WriteToSubresource(0, nullptr, data.data(), 4 * 4, 4 * 4 * 4);
}

void Model::CreateBlackTexture() {
	//黒テクスチャデータ
	std::vector<UCHAR> data(4 * 4 * 4);
	//真っ黒に染めちゃう
	std::fill(data.begin(), data.end(), 0x00);

	//書き込む
	auto result = blackBuffer->WriteToSubresource(0, nullptr, data.data(), 4 * 4, 4 * 4 * 4);
}


void Model::CreateBoneBuffer(ID3D12Device * dev) {
	//ボーンヒープ生成
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.Type			= D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	heapDesc.Flags			= D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	heapDesc.NumDescriptors = 1;
	heapDesc.NodeMask		= 0;

	auto result = dev->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&boneHeap));

	//ボーンバッファ生成
	CD3DX12_HEAP_PROPERTIES bHeapProp = {};
	bHeapProp.Type					= D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD;
	bHeapProp.MemoryPoolPreference	= D3D12_MEMORY_POOL::D3D12_MEMORY_POOL_UNKNOWN;
	bHeapProp.CPUPageProperty		= D3D12_CPU_PAGE_PROPERTY::D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	bHeapProp.CreationNodeMask		= 1;
	bHeapProp.VisibleNodeMask		= 1;

	D3D12_RESOURCE_DESC bResDesc = {};
	bResDesc.Flags				= D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_NONE;
	bResDesc.Format				= DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
	bResDesc.Layout				= D3D12_TEXTURE_LAYOUT::D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	bResDesc.Dimension			= D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_BUFFER;
	bResDesc.Height				= 1;
	bResDesc.Width				= ((sizeof(XMFLOAT3) + 0xff) &~0xff) * pmd.lock()->GetBoneData().size();
	bResDesc.DepthOrArraySize	= 1;
	bResDesc.Alignment			= 0;
	bResDesc.MipLevels			= 1;
	bResDesc.SampleDesc.Count	= 1;

	result = dev->CreateCommittedResource(
		&bHeapProp,
		D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
		&bResDesc,
		D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&boneBuffer)
	);

	D3D12_CONSTANT_BUFFER_VIEW_DESC bCbvDesc = {};
	bCbvDesc.BufferLocation = boneBuffer->GetGPUVirtualAddress();
	bCbvDesc.SizeInBytes	= ((sizeof(XMMATRIX) + 0xff) &~0xff) * pmd.lock()->GetBoneData().size();
	dev->CreateConstantBufferView(&bCbvDesc, boneHeap->GetCPUDescriptorHandleForHeapStart());

	result = boneBuffer->Map(0, nullptr, (void**)&boneMatrixData);
}

Model::~Model() {
}
