#pragma once


class RootSignature : public Result {
public:
	RootSignature();
	void InitRootSignature(UINT _size, D3D12_ROOT_PARAMETER* _parameter, D3D12_STATIC_SAMPLER_DESC _samplerDesc, ID3D12Device* _dev);
	ID3D12RootSignature* GetRootSignature();
	~RootSignature();
private:
	ID3D12RootSignature* rootSignature;
	ID3DBlob* signature;
	ID3DBlob* error;
	D3D12_ROOT_SIGNATURE_DESC rsDesc = {};
};

