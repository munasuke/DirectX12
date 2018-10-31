#include "Model.h"
#include "PMDLoader.h"
#include "ImageLoader.h"
#include "VMDMotion.h"
#include <iostream>

using namespace DirectX;

Model::Model(std::shared_ptr<PMDLoader> pmd, std::shared_ptr<ImageLoader> img, std::shared_ptr<VMDMotion> vmd) :
	pmd(pmd),
	img(img),
	vmd(vmd),
	heap(nullptr), 
	whiteBuffer(nullptr), 
	blackBuffer(nullptr),
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

	//白テクスチャ生成
	CreateWhiteTexture();

	//黒テクスチャ生成
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

	//ボーンバッファ生成
	CreateBoneBuffer(_dev);
}

void Model::Update() {
	static int frameNo = 0;
	MotionUpdate(frameNo / 2);
	++frameNo;
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

	//ボーンのセット
	_list->SetGraphicsRootConstantBufferView(2, boneBuffer->GetGPUVirtualAddress());

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

	auto result = dev->CreateCommittedResource(
		&bHeapProp,
		D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
		&bResDesc,
		D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&boneBuffer)
	);

	result = boneBuffer->Map(0, nullptr, (void**)&boneMatrixData);
}

void Model::RecursiveMatrixMultiply(BoneNode * node, DirectX::XMMATRIX & mat) {
	auto& boneMatrix = pmd.lock()->boneMatrices[node->boneIndex];
	boneMatrix *= mat;
	for (auto& cnode : node->children) {
		RecursiveMatrixMultiply(cnode, boneMatrix);
	}
}

void Model::RotationBone(const std::string str, const DirectX::XMFLOAT4 & angle, const DirectX::XMFLOAT4 & q2, float t) {
	auto node = pmd.lock()->boneMap[str];
	auto vec = XMLoadFloat3(&node.startPos);
	auto quaternion = XMLoadFloat4(&angle);
	auto quaternion2 = XMLoadFloat4(&q2);
	pmd.lock()->boneMatrices[node.boneIndex] = XMMatrixTranslationFromVector(
		XMVectorScale(vec, -1.0f))
		* XMMatrixRotationQuaternion(XMQuaternionSlerp(quaternion, quaternion2, t))
		* XMMatrixTranslationFromVector(vec);
}

void Model::MotionUpdate(int framNo) {
	auto& boneMatrix = pmd.lock()->boneMatrices;

	//ボーン行列の初期化
	std::fill(boneMatrix.begin(), boneMatrix.end(), XMMatrixIdentity());

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
	XMMATRIX root = XMMatrixIdentity();
	RecursiveMatrixMultiply(&pmd.lock()->boneMap["センター"], root);

	memcpy(boneMatrixData, boneMatrix.data(), ((sizeof(XMMATRIX) + 0xff) &~ 0xff) * pmd.lock()->GetBoneData().size());
}

float Model::GetVezierYValueFromXWithNewton(float x, DirectX::XMFLOAT2 a, DirectX::XMFLOAT2 b, unsigned int n) {
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

Model::~Model() {
}
