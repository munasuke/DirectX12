#pragma once
#include <d3d12.h>

//�T���v��
class Sampler
{
public:
	Sampler();

	void InitSampler();

	~Sampler();
private:
	D3D12_STATIC_SAMPLER_DESC samplerDesc = {};
};

