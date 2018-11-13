#pragma once
#include <vector>

class d3d12;

//パイプラインステートオブジェクト(OS)
class PipelineState : public Result{
public:
	PipelineState();
	//初期化
	void Initialize(ID3D12Device* _dev, CD3DX12_SHADER_BYTECODE _vs, CD3DX12_SHADER_BYTECODE _ps, UINT _num, D3D12_INPUT_ELEMENT_DESC* _input, ID3D12RootSignature* _root);
	void PeraInitialize(ID3D12Device* _dev, CD3DX12_SHADER_BYTECODE _vs, CD3DX12_SHADER_BYTECODE _ps, UINT _num, D3D12_INPUT_ELEMENT_DESC* _input, ID3D12RootSignature* _root, const unsigned int index);
	//ゲッター
	ID3D12PipelineState* GetPiplineState();
	std::vector<ID3D12PipelineState*> GetPeraPiplineState();
	~PipelineState();
private:
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpsDesc = {};
	ID3D12PipelineState* piplineState;
	std::vector<ID3D12PipelineState*> peraPipState;
};

