#pragma once

#include <d3d12.h>
#include <DirectXMath.h>

//���_�\����
struct VERTEX {
	DirectX::XMFLOAT3 pos;	//���W
	DirectX::XMFLOAT2 uv;	//uv���W
};

class Vertex : public Result {
public:
	Vertex();
	void Initialize(ID3D12Device* _dev);//������
	D3D12_INPUT_ELEMENT_DESC* GetInputDesc();
	UINT GetInputDescNum();
	~Vertex();
private:
	ID3D12Resource* vertexBuffer;//���_�o�b�t�@
	UCHAR* pData;
	D3D12_VERTEX_BUFFER_VIEW vbView = {};
};

