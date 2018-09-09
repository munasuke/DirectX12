#pragma once


class RootSignature : public Result {
public:
	RootSignature();
	void InitRootSignature(D3D12_STATIC_SAMPLER_DESC _samplerDesc, ID3D12Device* _dev, D3D12_SHADER_VISIBILITY _shaderVisibility);
	ID3D12RootSignature* GetRootSignature();
	~RootSignature();
private:
	D3D12_DESCRIPTOR_RANGE descriptorRange[1];
	D3D12_ROOT_PARAMETER parameter[1];

	ID3D12RootSignature* rootSignature;
	ID3DBlob* signature;
	ID3DBlob* error;
	D3D12_ROOT_SIGNATURE_DESC rsDesc = {};
};

