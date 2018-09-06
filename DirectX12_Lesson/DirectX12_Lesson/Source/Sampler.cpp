#include "Sampler.h"



Sampler::Sampler()
{
}

void Sampler::InitSampler()
{
	samplerDesc.Filter				= D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT;
	samplerDesc.AddressU			= D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.AddressV			= D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.AddressW			= D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.MaxLOD				= D3D12_FLOAT32_MAX;
	samplerDesc.MinLOD				= 0.0f;
	samplerDesc.MipLODBias			= 0.0f;
	samplerDesc.BorderColor			= D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	samplerDesc.ShaderRegister		= 0;
	samplerDesc.ShaderVisibility	= D3D12_SHADER_VISIBILITY_PIXEL;
	samplerDesc.RegisterSpace		= 0;
	samplerDesc.MaxAnisotropy		= 0;
	samplerDesc.ComparisonFunc		= D3D12_COMPARISON_FUNC_NEVER;
}


Sampler::~Sampler()
{
}
