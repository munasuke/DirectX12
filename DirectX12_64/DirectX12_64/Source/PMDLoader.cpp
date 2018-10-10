#include "PMDLoader.h"
#include "BmpLoader.h"
#include <tchar.h>
#include <stdio.h>
#include <iostream>
#include <string>

PMDLoader::PMDLoader() : resource(nullptr), descriptorHeap(nullptr), data(nullptr){
	bmp = std::make_shared<BmpLoader>();
	mat = {};
}

int PMDLoader::Load(const char * _path) {
	FILE* fp = nullptr;
	if (fopen_s(&fp, _path, "rb") != 0){
		MessageBox(nullptr, TEXT("Can't load PMDFile"), TEXT("Error"), MB_OK);
		return -1;
	}
	//ヘッダー読み込み
	fread(&header, sizeof(header), 1, fp);
	//fread(&pmd.header.magic, sizeof(pmd.header.magic), 1, fp);
	//fread(&pmd.header.version, sizeof(pmd.header) - sizeof(pmd.header.magic), 1, fp);
	
	//頂点データ読み込み
	vertices.resize(header.vertexNum);
	fread(&vertices[0], sizeof(PMDVertex), header.vertexNum, fp);

	//インデックス総数読み込み
	fread(&indicesNum, sizeof(UINT), 1, fp);
	//インデックスデータ読み込み
	indices.resize(indicesNum);
	fread(&indices[0], sizeof(USHORT), indicesNum, fp);

	//マテリアル総数読み込み
	fread(&materialNum, sizeof(UINT), 1, fp);
	//マテリアルデータ読み込み
	material.resize(materialNum);
	fread(&material[0], sizeof(PMDMaterial), materialNum, fp);
	
	fclose(fp);

	//テクスチャ読み込み
	//for (INT i = 0; i < material.size(); ++i){
	//	//テクスチャがある場合のみ読み込み
	//	if (material[i].textureFilePath[0] != '\0') {
	//		bmp->Load(material[i].textureFilePath);
	//	}
	//}
	bmp->Load("PMD/miku/eye2.bmp");

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

	//マテリアル分CBVを作成する
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

	//マテリアルをシェーダに渡す
	result = resource->Map(0, nullptr, (void**)(&data));
	for (UINT i = 0; i < material.size(); ++i) {
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

		//テクスチャのありなし判定
		mat.texFlag = material[i].textureFilePath[0] != '\0' ? true : false;
		texFlag.emplace_back(mat.texFlag);

		//テクスチャありのマテリアル番号を表示
		std::cout << i << ":" << std::boolalpha << (bool)(mat.texFlag) << std::endl;

		//マテリアル成分をGPUに投げる
		memcpy(data, &mat, sizeof(MAT));

		//データをずらす
		data = (UINT8*)(((sizeof(DirectX::XMFLOAT3) + 0xff) &~0xff) + (CHAR*)(data));
	}
}

void PMDLoader::Draw(ID3D12GraphicsCommandList * _list, ID3D12Device * _dev, ID3D12DescriptorHeap* texHeap) {
	UINT offset = 0;
	for (UINT i = 0; i < material.size(); ++i) {
		//テクスチャのデスクリプタをセット
		_list->SetDescriptorHeaps(1, &texHeap);
		_list->SetGraphicsRootDescriptorTable(0, texHeap->GetGPUDescriptorHandleForHeapStart());

		//マテリアルの数分デスクリプタをセット
		auto handle = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
		handle.ptr += i * _dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		_list->SetDescriptorHeaps(1, &descriptorHeap);
		_list->SetGraphicsRootDescriptorTable(2, handle);

		//マテリアル別に描画
		_list->DrawIndexedInstanced(material[i].indexCount, 1, offset, 0, 0);

		//マテリアルのインデックス分ずらす
		offset += material[i].indexCount;
	}
}

void PMDLoader::SetDescriptor(ID3D12GraphicsCommandList * _list, ID3D12Device* _dev) {
	auto handle = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
	for (UINT i = 0; i < material.size(); ++i){
		_list->SetDescriptorHeaps(1, &descriptorHeap);
		_list->SetGraphicsRootDescriptorTable(2, handle);
		handle.ptr += _dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}
}

void PMDLoader::SetDescriptor(ID3D12GraphicsCommandList * _list, ID3D12Device * _dev, UINT index) {
	auto handle = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
	handle.ptr += index * _dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	_list->SetDescriptorHeaps(1, &descriptorHeap);
	_list->SetGraphicsRootDescriptorTable(2, handle);
}

void PMDLoader::SetMaterialColor(UINT index) {
	//減衰色
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
	//テクスチャデータの読み込み
	//フォルダの区切りに/か\\のどちらかが使われる

	std::string strModelPath = modelPath;
	std::string strTexPath = texturePath;

	INT path1 = strModelPath.rfind('/');
	INT path2 = strModelPath.rfind('\\') == std::string::npos ? 0 : strModelPath.rfind('\\');
	//フォルダの区切りにどちらが使われているか比較する
	INT pathIndex = max(path1, path2);
	//アプリからの相対パスを生成
	std::string _texturePath = strModelPath.substr(0, pathIndex);
	_texturePath += '/' + texturePath;

	return _texturePath;
}

UINT8 * PMDLoader::GetData(void) {
	return data;
}

Rect PMDLoader::GetBMPSize() {
	Rect tmpRec = { bmp->GetInfoHeader().biWidth, bmp->GetInfoHeader().biHeight };

	return tmpRec;
}

std::vector<CHAR> PMDLoader::GetBMPData() {
	return bmp->GetData();
}

void PMDLoader::UpdateData() {
	data = (UINT8*)(((sizeof(DirectX::XMFLOAT3) + 0xff) &~0xff) + (CHAR*)(data));
}

PMDLoader::~PMDLoader() {
}
