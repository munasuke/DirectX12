#include "Result.h"
#include "Vertex.h"

namespace {
	//���_���
	VERTEX vertices[] = {
		{ { -1.0f,  1.0f, 0.0f },{ 0.0f, 0.0f } },
		{ { 1.0f,  1.0f, 0.0f },{ 1.0f, 0.0f } },
		{ { 1.0f, -1.0f, 0.0f },{ 1.0f, 1.0f } },

		{ { 1.0f, -1.0f, 0.0f },{ 1.0f, 1.0f } },
		{ { -1.0f, -1.0f, 0.0f },{ 0.0f, 1.0f } },
		{ { -1.0f,  1.0f, 0.0f },{ 0.0f, 0.0f } }
	};

	//���_���C�A�E�g
	D3D12_INPUT_ELEMENT_DESC inputDesc[] = {
		//���_
		{
			"POSITION",									//SemanticName
			0,											//SemanticIndex
			DXGI_FORMAT_R32G32B32_FLOAT,				//Format
			0,											//InputSlot
			D3D12_APPEND_ALIGNED_ELEMENT,				//AlignedByteoffset
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, //InputSlotClass
			0											//InstanceDataStepRate
		},
		//uv
		{
			"TEXCOORD",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		}
	};
}

Vertex::Vertex() : vertexBuffer(nullptr), pData(nullptr)
{
}

void Vertex::Initialize(ID3D12Device * _dev)
{
	//���_�o�b�t�@�̍쐬
	result = _dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),//CPU����GPU�֓]������p
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(vertices)),//�T�C�Y
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertexBuffer));

	//�o�b�t�@�ɑ΂��ď�������
	result = vertexBuffer->Map(0, nullptr, (void**)&pData);
	memcpy(pData, vertices, sizeof(vertices));//���_�f�[�^���o�b�t�@�ɃR�s�[
	vertexBuffer->Unmap(0, nullptr);

	//���_�o�b�t�@�r���[�̍쐬
	vbView.BufferLocation	= vertexBuffer->GetGPUVirtualAddress();	//���_�A�h���X��GPU�ɂ���A�h���X���L��
	vbView.StrideInBytes	= sizeof(VERTEX);						//���_1������̃o�C�g�����w��
	vbView.SizeInBytes		= sizeof(vertices);						//�f�[�^�S�̂̃T�C�Y���w��
}

D3D12_INPUT_ELEMENT_DESC * Vertex::GetInputDesc() {
	return inputDesc;
}

UINT Vertex::GetInputDescNum() {
	return _countof(inputDesc);
}

D3D12_VERTEX_BUFFER_VIEW Vertex::GetVBV() {
	return vbView;
}


Vertex::~Vertex()
{
	Release(vertexBuffer);
}
