#include "Result.h"
#include "PipelineState.h"
#include <d3d12.h>


PipelineState::PipelineState() : piplineState(nullptr){
}

void PipelineState::Initialize(ID3D12Device* _dev, CD3DX12_SHADER_BYTECODE _vs, CD3DX12_SHADER_BYTECODE _ps, UINT _num, D3D12_INPUT_ELEMENT_DESC* _input, ID3D12RootSignature* _root) {
	gpsDesc.BlendState							= CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	gpsDesc.DepthStencilState.DepthEnable		= false;
	gpsDesc.DepthStencilState.StencilEnable		= false;
	gpsDesc.VS									= _vs;
	gpsDesc.PS									= _ps;
	gpsDesc.InputLayout.NumElements				= _num;
	gpsDesc.InputLayout.pInputElementDescs		= _input;
	gpsDesc.pRootSignature						= _root;
	gpsDesc.RasterizerState						= CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	gpsDesc.RTVFormats[0]						= DXGI_FORMAT_R8G8B8A8_UNORM;
	gpsDesc.PrimitiveTopologyType				= D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	gpsDesc.SampleDesc.Count					= 1;
	gpsDesc.NumRenderTargets					= 1;
	gpsDesc.SampleMask							= UINT_MAX;

	result = _dev->CreateGraphicsPipelineState(&gpsDesc, IID_PPV_ARGS(&piplineState));
}


PipelineState::~PipelineState() {
}
