#include "Result.h"
#include "RootSignature.h"



RootSignature::RootSignature() : 
	rootSignature(nullptr),
	signature(nullptr),
	error(nullptr)
{
	SecureZeroMemory(&descriptorRange, sizeof(descriptorRange));
	SecureZeroMemory(&parameter, sizeof(parameter));
}

void RootSignature::InitRootSignature(D3D12_STATIC_SAMPLER_DESC _samplerDesc, ID3D12Device* _dev, D3D12_SHADER_VISIBILITY _shaderVisibility) {
	//デスクリプタレンジ
	descriptorRange[0].RangeType							= D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange[0].NumDescriptors						= 1;
	descriptorRange[0].BaseShaderRegister					= 0;
	descriptorRange[0].RegisterSpace						= 0;
	descriptorRange[0].OffsetInDescriptorsFromTableStart	= D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	descriptorRange[1].RangeType							= D3D12_DESCRIPTOR_RANGE_TYPE::D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	descriptorRange[1].NumDescriptors						= 1;
	descriptorRange[1].BaseShaderRegister					= 0;
	descriptorRange[1].RegisterSpace						= 0;
	descriptorRange[1].OffsetInDescriptorsFromTableStart	= D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	//パラメータ
	parameter[0].ParameterType							= D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	parameter[0].ShaderVisibility						= _shaderVisibility;
	parameter[0].DescriptorTable.NumDescriptorRanges	= 1;
	parameter[0].DescriptorTable.pDescriptorRanges		= &descriptorRange[0];

	parameter[1].ParameterType							= D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	parameter[1].ShaderVisibility						= D3D12_SHADER_VISIBILITY_VERTEX;
	parameter[1].DescriptorTable.NumDescriptorRanges	= 1;
	parameter[1].DescriptorTable.pDescriptorRanges		= &descriptorRange[1];

	//ルートシグネチャ
	rsDesc.NumParameters		= _countof(parameter);
	rsDesc.NumStaticSamplers	= 1;
	rsDesc.pParameters			= parameter;
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

ID3DBlob * RootSignature::GetError() {
	return error;
}


RootSignature::~RootSignature() {
	Release(error);
	Release(signature);
	Release(rootSignature);
}
