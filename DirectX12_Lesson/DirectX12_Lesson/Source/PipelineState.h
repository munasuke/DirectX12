#pragma once

class d3d12;

//パイプラインステートオブジェクト(OS)
class PipelineState : public Result{
public:
	PipelineState();
	void Initialize(ID3D12Device* _dev, CD3DX12_SHADER_BYTECODE _vs, CD3DX12_SHADER_BYTECODE _ps, UINT _num, D3D12_INPUT_ELEMENT_DESC* _input, ID3D12RootSignature* _root);
	~PipelineState();
private:
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpsDesc = {};
	ID3D12PipelineState* piplineState;
};

