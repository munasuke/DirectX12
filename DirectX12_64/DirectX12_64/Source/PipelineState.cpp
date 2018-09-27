#include "Result.h"
#include "PipelineState.h"
#include <d3d12.h>


PipelineState::PipelineState() : piplineState(nullptr){
}

void PipelineState::Initialize(ID3D12Device* _dev, CD3DX12_SHADER_BYTECODE _vs, CD3DX12_SHADER_BYTECODE _ps, UINT _num, D3D12_INPUT_ELEMENT_DESC* _input, ID3D12RootSignature* _root) {
	gpsDesc.BlendState							= CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	gpsDesc.DepthStencilState.DepthEnable		= true;													//�[�x�o�b�t�@��L���ɂ���
	gpsDesc.DepthStencilState.DepthWriteMask	= D3D12_DEPTH_WRITE_MASK::D3D12_DEPTH_WRITE_MASK_ALL;	//��ɐ[�x�l����������
	gpsDesc.DepthStencilState.DepthFunc			= D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_LESS;	//�[�x�l�̌��ʂ���������(�J�����ɋ߂���)����������
	gpsDesc.DSVFormat							= DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT;					//�[�x�o�b�t�@�̃t�H�[�}�b�g�𖾎�����
	gpsDesc.DepthStencilState.StencilEnable		= false;
	gpsDesc.VS									= _vs;
	gpsDesc.PS									= _ps;
	gpsDesc.InputLayout.NumElements				= _num;
	gpsDesc.InputLayout.pInputElementDescs		= _input;
	gpsDesc.pRootSignature						= _root;
	gpsDesc.RasterizerState						= CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	gpsDesc.RTVFormats[0]						= DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
	gpsDesc.PrimitiveTopologyType				= D3D12_PRIMITIVE_TOPOLOGY_TYPE::D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	gpsDesc.SampleDesc.Count					= 1;
	gpsDesc.NumRenderTargets					= 1;
	gpsDesc.SampleMask							= UINT_MAX;

	result = _dev->CreateGraphicsPipelineState(&gpsDesc, IID_PPV_ARGS(&piplineState));
}

ID3D12PipelineState * PipelineState::GetPiplineState()
{
	return piplineState;
}


PipelineState::~PipelineState() {
}
