#include "Result.h"
#include "RootSignature.h"



RootSignature::RootSignature() : 
	rootSignature(nullptr),
	signature(nullptr),
	error(nullptr)
{
}

void RootSignature::InitRootSignature(UINT _size, D3D12_ROOT_PARAMETER * _parameter, D3D12_STATIC_SAMPLER_DESC _samplerDesc, ID3D12Device* _dev) {
	rsDesc.NumParameters		= _size;
	rsDesc.NumStaticSamplers	= 1;
	rsDesc.pParameters			= _parameter;
	rsDesc.pStaticSamplers		= &_samplerDesc;
	rsDesc.Flags				= D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	result = D3D12SerializeRootSignature(
		&rsDesc,
		D3D_ROOT_SIGNATURE_VERSION_1,
		&signature,
		&error
	);
	result = _dev->CreateRootSignature(
		0,
		signature->GetBufferPointer(),
		signature->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature)
	);
}

ID3D12RootSignature * RootSignature::GetRootSignature() {
	return rootSignature;
}


RootSignature::~RootSignature() {
}
