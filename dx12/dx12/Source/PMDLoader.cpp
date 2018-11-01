#include "PMDLoader.h"
#include "BmpLoader.h"
#include "ImageLoader.h"
#include "VMDMotion.h"
#include <tchar.h>
#include <stdio.h>
#include <iostream>
#include <algorithm>
#include <windows.h>

#pragma comment(lib, "winmm.lib")

using namespace DirectX;

PMDLoader::PMDLoader(std::shared_ptr<BmpLoader> bmp, std::shared_ptr<ImageLoader> imageL, std::shared_ptr<VMDMotion> vmd) :
	textureNum(0),
	resource(nullptr),
	descriptorHeap(nullptr),
	data(nullptr),
	matrixData(nullptr)
{
	this->bmp = bmp;
	this->imageL = imageL;
	this->vmd = vmd;
	mat = {};
}

int PMDLoader::Load(const char * _path) {
	std::cout << _path << std::endl;
	FILE* fp = nullptr;
	if (fopen_s(&fp, _path, "rb") != 0) {
		MessageBox(nullptr, TEXT("Can't load PMDFile"), TEXT("Error"), MB_OK);
		return -1;
	}
	//ヘッダー読み込み
	fread(&header, sizeof(header), 1, fp);
	//fread(&pmd.header.magic, sizeof(pmd.header.magic), 1, fp);
	//fread(&pmd.header.version, sizeof(pmd.header) - sizeof(pmd.header.magic), 1, fp);

	//頂点情報読み込み
	vertices.resize(header.vertexNum);
	fread(&vertices[0], sizeof(PMDVertex), header.vertexNum, fp);

	//インデックス総数
	fread(&indicesNum, sizeof(UINT), 1, fp);
	//インデックス情報読み込み
	indices.resize(indicesNum);
	fread(&indices[0], sizeof(USHORT), indicesNum, fp);

	//マテリアル総数
	fread(&materialNum, sizeof(UINT), 1, fp);
	//マテリアル情報読み込み
	material.resize(materialNum);
	fread(&material[0], sizeof(PMDMaterial), materialNum, fp);

	//ボーン総数
	fread(&boneCount, sizeof(boneCount), 1, fp);
	//ボーン情報読み込み
	bone.resize(boneCount);
	for (auto& b : bone) {
		fread(&b.boneName, sizeof(b.boneName), 1, fp);
		fread(&b.parentBoneIndex, sizeof(b.parentBoneIndex), 1, fp);
		fread(&b.tailPosBoneIndex, sizeof(b.tailPosBoneIndex), 1, fp);
		fread(&b.boneType, sizeof(b.boneType), 1, fp);
		fread(&b.ikParentBoneIndex, sizeof(b.ikParentBoneIndex), 1, fp);
		fread(&b.boneHeadPos, sizeof(b.boneHeadPos), 1, fp);
	}

	fclose(fp);

	for (INT i = 0; i < material.size(); ++i) {
		if (strlen(material[i].textureFilePath) > 0) {
			++textureNum;
			auto texPath = GetRelativeTexturePathFromPmdPath(_path, material[i].textureFilePath);
			imageL.lock()->Load(texPath.c_str());
		}
	}

	//ボーン初期化
	boneMatrices.resize(bone.size());
	std::fill(boneMatrices.begin(), boneMatrices.end(), DirectX::XMMatrixIdentity());

	//ボーンノードマップの形成
	for (UINT i = 0; i < bone.size(); ++i) {
		auto& pb = bone[i];
		auto& boneNode = boneMap[pb.boneName];
		boneNode.boneIndex = i;
		boneNode.startPos = pb.boneHeadPos;
	}

	//ボーンツリー形成
	for (auto& pb : bone) {
		if (pb.parentBoneIndex >= bone.size()) {
			continue;
		}
		auto pName = bone[pb.parentBoneIndex].boneName;
		boneMap[pName].children.emplace_back(&boneMap[pb.boneName]);
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


	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc = {};
	cbvHeapDesc.NumDescriptors = material.size();
	cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	cbvHeapDesc.NodeMask = 0;
	cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	_dev->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&descriptorHeap));

	//ボーン
	CreateBoneBuffer(_dev);

	auto handle = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
	auto address = resource->GetGPUVirtualAddress();
	for (UINT i = 0; i < material.size(); ++i) {
		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
		cbvDesc.BufferLocation = address;
		cbvDesc.SizeInBytes = ((sizeof(DirectX::XMFLOAT3) + 0xff) &~0xff);
		_dev->CreateConstantBufferView(&cbvDesc, handle);

		address += cbvDesc.SizeInBytes;
		handle.ptr += _dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}

	result = resource->Map(0, nullptr, (void**)(&data));
	for (UINT i = 0; i < material.size(); ++i) {
		mat.diffuse.x = material[i].diffuse.x;
		mat.diffuse.y = material[i].diffuse.y;
		mat.diffuse.z = material[i].diffuse.z;
		mat.diffuse.w = material[i].alpha;

		mat.specular.x = material[i].specular.x;
		mat.specular.y = material[i].specular.y;
		mat.specular.z = material[i].specular.z;
		mat.specular.w = material[i].specularPower;

		mat.ambient.x = material[i].ambient.x;
		mat.ambient.y = material[i].ambient.y;
		mat.ambient.z = material[i].ambient.z;

		mat.texFlag = material[i].textureFilePath[0] != '\0' ? true : false;
		texFlag.emplace_back(mat.texFlag);

		std::cout << i << ":" << std::boolalpha << (bool)(mat.texFlag) << std::endl;

		memcpy(data, &mat, sizeof(MAT));

		data = (UINT8*)(((sizeof(DirectX::XMFLOAT3) + 0xff) &~0xff) + (CHAR*)(data));
	}
}

//再帰
void PMDLoader::RecursiveMatrixMultiply(BoneNode* node, DirectX::XMMATRIX& mat) {
	boneMatrices[node->boneIndex] *= mat;
	for (auto& cnode : node->children) {
		RecursiveMatrixMultiply(cnode, boneMatrices[node->boneIndex]);
	}
}

void PMDLoader::RotationBone(const std::string str, const XMFLOAT4& angle, const XMFLOAT4& q2, float t) {
	auto node = boneMap[str];
	auto vec = XMLoadFloat3(&node.startPos);
	auto quaternion = XMLoadFloat4(&angle);
	auto quaternion2 = XMLoadFloat4(&q2);
	boneMatrices[node.boneIndex] = XMMatrixTranslationFromVector(
		XMVectorScale(vec, -1.0f))
		* XMMatrixRotationQuaternion(XMQuaternionSlerp(quaternion, quaternion2, t))
		* XMMatrixTranslationFromVector(vec);
}

void PMDLoader::Update() {
	static auto lastTime = timeGetTime();
	float fps = 33.33333f;
	MotionUpdate(static_cast<float>(timeGetTime() - lastTime) / fps);
	if (static_cast<float>(timeGetTime() - lastTime) > vmd.lock()->GetDuration() * fps) {
		lastTime = timeGetTime();
	}
}

void PMDLoader::Draw(ID3D12GraphicsCommandList * _list, ID3D12Device * _dev, ID3D12DescriptorHeap* texHeap) {
	UINT offset = 0;

	//_list->SetDescriptorHeaps(1, &boneHeap);
	//_list->SetGraphicsRootDescriptorTable(3, boneHeap->GetGPUDescriptorHandleForHeapStart());
	_list->SetGraphicsRootConstantBufferView(3, boneBuffer->GetGPUVirtualAddress());

	for (UINT i = 0; i < material.size(); ++i) {
		_list->SetDescriptorHeaps(1, &texHeap);
		auto texHandle = texHeap->GetGPUDescriptorHandleForHeapStart();
		_list->SetGraphicsRootDescriptorTable(0, texHandle);

		auto handle = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
		handle.ptr += i * _dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		_list->SetDescriptorHeaps(1, &descriptorHeap);
		_list->SetGraphicsRootDescriptorTable(2, handle);

		_list->DrawIndexedInstanced(material[i].indexCount, 1, offset, 0, 0);

		offset += material[i].indexCount;
	}
}

void PMDLoader::MotionUpdate(int framNo) {
	std::fill(boneMatrices.begin(), boneMatrices.end(), XMMatrixIdentity());

	//データからポージング適用
	for (auto& anim : vmd.lock()->GetAnimationData()) {
		auto keyFlame = anim.second;
		auto frameIt = std::find_if(keyFlame.rbegin(), keyFlame.rend(),
				[framNo](const MotionData& m) { return m.frameNo <= framNo; });
		if (frameIt == keyFlame.rend()) {
			continue;
		}
		auto nextIt = frameIt.base();
		if (nextIt == keyFlame.end()) {
			RotationBone(anim.first.c_str(), frameIt->quaternion);
		}
		else {
			float a = frameIt->frameNo;
			float b = nextIt->frameNo;
			float t = static_cast<float>(framNo - a) / (b - a);
			t = GetVezierYValueFromXWithNewton(t, frameIt->bz[0], frameIt->bz[1]);
			RotationBone(anim.first.c_str(), frameIt->quaternion, nextIt->quaternion, t);
		}
	}
	//ツリーをトラバース
	auto& boneMat = boneMatrices[boneMap["センター"].boneIndex];
	RecursiveMatrixMultiply(&boneMap["センター"], boneMat);

	memcpy(matrixData, boneMatrices.data(), ((sizeof(XMMATRIX) + 0xff) &~ 0xff) * bone.size());
}

void PMDLoader::SetMaterialColor(UINT index) {
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
	std::string strModelPath = modelPath;

	auto index1 = strModelPath.rfind('/');
	auto index2 = strModelPath.rfind('\\');
	index2 = std::string::npos ? 0 : index2;

	auto separation = max(index1, index2);

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

void PMDLoader::CreateBoneBuffer(ID3D12Device * _dev) {
	//ボーンヒープ
	//D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc = {};
	//cbvHeapDesc.NumDescriptors	= 1;
	//cbvHeapDesc.Flags			= D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	//cbvHeapDesc.NodeMask		= 0;
	//cbvHeapDesc.Type			= D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	//auto result = _dev->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&boneHeap));

	//バッファ生成
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
	resourceDesc.Width				= ((sizeof(DirectX::XMFLOAT3) + 0xff) &~0xff) * bone.size();
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
		IID_PPV_ARGS(&boneBuffer)
	);

	//ビュー
	//D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	//cbvDesc.BufferLocation	= boneBuffer->GetGPUVirtualAddress();
	//cbvDesc.SizeInBytes		= ((sizeof(DirectX::XMMATRIX) + 0xff) &~ 0xff) * bone.size();
	//_dev->CreateConstantBufferView(&cbvDesc, boneHeap->GetCPUDescriptorHandleForHeapStart());

	result = boneBuffer->Map(0, nullptr, (void**)&matrixData);

	//boneBuffer->Unmap(0, nullptr);
}

float PMDLoader::GetVezierYValueFromXWithNewton(float x, DirectX::XMFLOAT2 a, DirectX::XMFLOAT2 b, unsigned int n) {
	//直線だった場合は計算せずに抜ける
	if (a.x == a.y && b.x == b.y) {
		return x;
	}

	//最終的に求めたい媒介変数
	float t = x;				//初期値はxと同じ

	//係数
	float k[] = {
		1 + 3 * a.x - 3 * b.x,	//t^3の係数
		3 * b.x - 6 * a.x,		//t^2の係数
		3 * a.x					//tの係数
	};

	//誤差の範囲内かどうかに使用する定数
	const float epsilon = 0.0005f;

	//ニュートン法
	for (int i = 0; i < n; ++i) {
		//f(t)を求める
		float ft = (t * t * t) * k[0] + (t * t) * k[1] + t * k[2] - x;

		//誤差の範囲内ならば計算終了
		if (ft <= epsilon && ft >= -epsilon) {
			break;
		}

		//f'(t)を求める(f(t)の微分結果)
		float fdt = 3 * (t * t) * k[0] + 2 * t * k[1] + k[2];

		//0除算を防止する
		if (fdt == 0) {
			break;
		}

		//答に近づける
		t = t - ft / fdt;
	}

	//反転
	float r = 1 - t;

	return 3 * (r * r * t * a.y) + 3 * (r * t * t * b.y) + (t * t * t);
}
