#include "Result.h"
#include "Vertex.h"
#include <algorithm>

namespace {
	//頂点情報
	VERTEX vertices[] = {
		{ { -1.0f, -1.0f, 0.0f },{ 0.0f, 1.0f } },
		{ { -1.0f,  1.0f, 0.0f },{ 0.0f, 0.0f } },
		{ {  1.0f, -1.0f, 0.0f },{ 1.0f, 1.0f } },
		{ {  1.0f,  1.0f, 0.0f },{ 1.0f, 0.0f } }
	};

	//頂点レイアウト
	D3D12_INPUT_ELEMENT_DESC inputDesc[] = {
		//頂点
		{
			"POSITION",																//SemanticName
			0,																		//SemanticIndex
			DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT,								//Format
			0,																		//InputSlot
			D3D12_APPEND_ALIGNED_ELEMENT,											//AlignedByteoffset
			D3D12_INPUT_CLASSIFICATION::D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, //InputSlotClass
			0																		//InstanceDataStepRate
		},
		//法線
		{
			"NORMAL",
			0,
			DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION::D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		},
		//UV
		{
			"TEXCOORD",
			0,
			DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION::D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		},
		//ボーン
		{
			"BONENO",
				0,
				DXGI_FORMAT::DXGI_FORMAT_R16G16_UINT,
				0,
				D3D12_APPEND_ALIGNED_ELEMENT,
				D3D12_INPUT_CLASSIFICATION::D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
				0
		},
		//ウエイト
		{
			"WEIGHT",
			0,
			DXGI_FORMAT::DXGI_FORMAT_R8_UINT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION::D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		}
	};

	//ペラポリゴン用
	D3D12_INPUT_ELEMENT_DESC peraInput[] = {
		{
			"POSITION",
			0,
			DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION::D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		},
		{
			"TEXCOORD",
			0,
			DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION::D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		}
	};
}

Vertex::Vertex() : vertexBuffer(nullptr), pData(nullptr)
{
}

void Vertex::Initialize(ID3D12Device * _dev, std::vector<PMDVertex> _pmdV)
{
	//頂点バッファの作成
	result = _dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD),//CPUからGPUへ転送する用
		D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(PMDVertex) * _pmdV.size()),//サイズ
		D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertexBuffer));

	//バッファに対して書き込む
	result = vertexBuffer->Map(0, nullptr, (void**)&pData);
	memcpy(pData, &_pmdV[0], sizeof(PMDVertex) * _pmdV.size());	//頂点データをバッファにコピー
	vertexBuffer->Unmap(0, nullptr);

	//頂点バッファビューの作成
	vbView.BufferLocation	= vertexBuffer->GetGPUVirtualAddress();	//頂点アドレスのGPUにあるアドレスを記憶
	vbView.StrideInBytes	= sizeof(PMDVertex);					//頂点1つあたりのバイト数を指定
	vbView.SizeInBytes		= sizeof(PMDVertex) * _pmdV.size();		//データ全体のサイズを指定

	//ペラポリゴン用
	//ペラバッファ生成
	ID3D12Resource* peraBuffer[2];
	for (int i = 0; i < 2; i++) {
		peraBuffer[i] = nullptr;
	}
	//1パス目
	result = _dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(vertices)),
		D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&peraBuffer[0])
	);

	VERTEX* vData = nullptr;
	result = peraBuffer[0]->Map(0, nullptr, (void**)&vData);
	memcpy(vData, vertices, sizeof(vertices));
	peraBuffer[0]->Unmap(0, nullptr);

	peraVBV[0].BufferLocation = peraBuffer[0]->GetGPUVirtualAddress();
	peraVBV[0].SizeInBytes = sizeof(vertices);
	peraVBV[0].StrideInBytes = sizeof(VERTEX);

	//2パス目
	result = _dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(vertices)),
		D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&peraBuffer[1])
	);

	vData = nullptr;
	result = peraBuffer[1]->Map(0, nullptr, (void**)&vData);
	memcpy(vData, vertices, sizeof(vertices));
	peraBuffer[1]->Unmap(0, nullptr);

	peraVBV[1].BufferLocation = peraBuffer[1]->GetGPUVirtualAddress();
	peraVBV[1].SizeInBytes = sizeof(vertices);
	peraVBV[1].StrideInBytes = sizeof(VERTEX);
}

D3D12_INPUT_ELEMENT_DESC * Vertex::GetInputDesc() {
	return inputDesc;
}

D3D12_INPUT_ELEMENT_DESC * Vertex::GetPeraInputDesc() {
	return peraInput;
}

UINT Vertex::GetInputDescNum() {
	return _countof(inputDesc);
}

UINT Vertex::GetPeraInputDescNum() {
	return _countof(peraInput);
}

D3D12_VERTEX_BUFFER_VIEW Vertex::GetVBV() {
	return vbView;
}

D3D12_VERTEX_BUFFER_VIEW Vertex::GetPeraVBV(const unsigned int index) {
	return peraVBV[index];
}


Vertex::~Vertex()
{
	ReleaseP(vertexBuffer);
}
