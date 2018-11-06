#pragma once

class d3d12;

//�p�C�v���C���X�e�[�g�I�u�W�F�N�g(OS)
class PipelineState : public Result{
public:
	PipelineState();
	//������
	void Initialize(ID3D12Device* _dev, CD3DX12_SHADER_BYTECODE _vs, CD3DX12_SHADER_BYTECODE _ps, UINT _num, D3D12_INPUT_ELEMENT_DESC* _input, ID3D12RootSignature* _root);
	void PeraInitialize(ID3D12Device* _dev, CD3DX12_SHADER_BYTECODE _vs, CD3DX12_SHADER_BYTECODE _ps, UINT _num, D3D12_INPUT_ELEMENT_DESC* _input, ID3D12RootSignature* _root);
	//�Q�b�^�[
	ID3D12PipelineState* GetPiplineState();
	ID3D12PipelineState* GetPeraPiplineState();
	~PipelineState();
private:
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpsDesc = {};
	ID3D12PipelineState* piplineState;
	ID3D12PipelineState* peraPipState;
};

