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
	/*
		デスクリプタレンジ
	*/
	//t0(通常テクスチャ)
	descriptorRange[0].RangeType							= D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange[0].NumDescriptors						= 1;
	descriptorRange[0].BaseShaderRegister					= 0;
	descriptorRange[0].RegisterSpace						= 0;
	descriptorRange[0].OffsetInDescriptorsFromTableStart	= D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	//b0(カメラ)
	descriptorRange[1].RangeType							= D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	descriptorRange[1].NumDescriptors						= 1;
	descriptorRange[1].BaseShaderRegister					= 0;
	descriptorRange[1].RegisterSpace						= 0;
	descriptorRange[1].OffsetInDescriptorsFromTableStart	= D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	//b1(マテリアル情報)
	descriptorRange[2].RangeType							= D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	descriptorRange[2].NumDescriptors						= 1;
	descriptorRange[2].BaseShaderRegister					= 1;
	descriptorRange[2].RegisterSpace						= 0;
	descriptorRange[2].OffsetInDescriptorsFromTableStart	= D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	descriptorRange[3].RangeType							= D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	descriptorRange[3].NumDescriptors						= 1;
	descriptorRange[3].BaseShaderRegister					= 2;
	descriptorRange[3].RegisterSpace						= 0;
	descriptorRange[3].OffsetInDescriptorsFromTableStart	= D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;


	/*
		パラメータ
	*/
	//b0(カメラ)
	parameter[0].ParameterType							= D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	parameter[0].ShaderVisibility						= D3D12_SHADER_VISIBILITY_ALL;
	parameter[0].DescriptorTable.NumDescriptorRanges	= 1;
	parameter[0].DescriptorTable.pDescriptorRanges		= &descriptorRange[0];

	//b1(マテリアル情報)
	parameter[1].ParameterType							= D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	parameter[1].ShaderVisibility						= D3D12_SHADER_VISIBILITY_ALL;
	parameter[1].DescriptorTable.NumDescriptorRanges	= 1;
	parameter[1].DescriptorTable.pDescriptorRanges		= &descriptorRange[1];

	parameter[2].ParameterType							= D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	parameter[2].ShaderVisibility						= D3D12_SHADER_VISIBILITY_ALL;
	parameter[2].DescriptorTable.NumDescriptorRanges	= 1;
	parameter[2].DescriptorTable.pDescriptorRanges		= &descriptorRange[2];

	parameter[3].ParameterType							= D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	parameter[3].ShaderVisibility						= D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_VERTEX;
	parameter[3].DescriptorTable.NumDescriptorRanges	= 1;
	parameter[3].DescriptorTable.pDescriptorRanges		= &descriptorRange[3];

	/*
		ルートシグネチャ
	*/
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
	ReleaseP(error);
	ReleaseP(signature);
	ReleaseP(rootSignature);
}
