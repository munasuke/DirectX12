#pragma once
#include "d3dx12.h"
#include <map>
#include <string>

class ShadowMap {
public:
	ShadowMap(ID3D12Device* dev, unsigned int inputDescNum, D3D12_INPUT_ELEMENT_DESC* inputDesc);

	void Draw(ID3D12GraphicsCommandList* list, const unsigned int materialNum);

	~ShadowMap();
private:
	size_t Size(size_t size);

	std::map<std::string, ID3D12DescriptorHeap*> heap;
	ID3D12Resource* resource;
	ID3D12PipelineState* gps;
	ID3D12RootSignature* rs;

};

