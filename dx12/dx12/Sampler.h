#pragma once
#include <d3d12.h>

//ƒTƒ“ƒvƒ‰
class Sampler
{
public:
	Sampler();

	void InitSampler();
	D3D12_STATIC_SAMPLER_DESC GetSamplerDesc();

	~Sampler();
private:
	D3D12_STATIC_SAMPLER_DESC samplerDesc = {};
};

