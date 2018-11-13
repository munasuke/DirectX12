#include "Result.h"
#include "RenderTarget.h"
#include "Window.h"
#include "SwapChain.h"
#include "Descriptor.h"


RenderTarget::RenderTarget()
{
}

void RenderTarget:: InitRenderTarget(UINT _bufferCnt, ID3D12Device* _dev, IDXGISwapChain3* _swapChain, CD3DX12_CPU_DESCRIPTOR_HANDLE _descriptorHandle, UINT _descriptorSize)
{
	renderNum = _bufferCnt;
	//レンダーターゲットの数ぶん確保
	renderTarget.resize(renderNum);

	for (UINT i = 0; i < renderNum; ++i) {
		//スワップチェインからキャンバスを取得
		result = _swapChain->GetBuffer(i, IID_PPV_ARGS(&renderTarget[i]));
		//キャンバスとビューを紐づけ
		_dev->CreateRenderTargetView(renderTarget[i], nullptr, _descriptorHandle);
		//キャンバスとビューのぶんだけ次のところまでオフセット
		_descriptorHandle.Offset(_descriptorSize);
	}
}

//1パス目
void RenderTarget::Init1stPathRTVSRV(ID3D12Device* _dev) {
	//ペラポリ用
	D3D12_DESCRIPTOR_HEAP_DESC descHeap = {};
	descHeap.Flags			= D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	descHeap.NodeMask		= 0;
	descHeap.NumDescriptors = 1;

	//RTV用ヒープ
	descHeap.Type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	_dev->CreateDescriptorHeap(&descHeap, IID_PPV_ARGS(&heapFor1stPath["RTV"]));

	//SRV用ヒープ
	descHeap.Type	= D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeap.Flags	= D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	_dev->CreateDescriptorHeap(&descHeap, IID_PPV_ARGS(&heapFor1stPath["SRV"]));

	D3D12_HEAP_PROPERTIES hProp = {};
	hProp.Type					= D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_DEFAULT;
	hProp.CPUPageProperty		= D3D12_CPU_PAGE_PROPERTY::D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	hProp.MemoryPoolPreference	= D3D12_MEMORY_POOL::D3D12_MEMORY_POOL_UNKNOWN;
	hProp.CreationNodeMask		= 1;
	hProp.VisibleNodeMask		= 1;

	D3D12_RESOURCE_DESC rDesc = {};
	rDesc.Height			= WIN_HEIGHT;
	rDesc.Width				= WIN_WIDTH;
	rDesc.MipLevels			= 1;
	rDesc.DepthOrArraySize	= 1;
	rDesc.SampleDesc.Count	= 1;
	rDesc.Flags				= D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	rDesc.Dimension			= D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	rDesc.Layout			= D3D12_TEXTURE_LAYOUT::D3D12_TEXTURE_LAYOUT_UNKNOWN;
	rDesc.Format			= DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;

	//RTVSRVバッファ生成
	result = _dev->CreateCommittedResource(
		&hProp,
		D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
		&rDesc,
		D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_RENDER_TARGET,
		nullptr,
		IID_PPV_ARGS(&peraBuffer)
	);

	//RTV生成
	_dev->CreateRenderTargetView(peraBuffer, nullptr, heapFor1stPath["RTV"]->GetCPUDescriptorHandleForHeapStart());

	//SRV生成
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format					= DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension			= D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels		= 1;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	_dev->CreateShaderResourceView(peraBuffer, &srvDesc, heapFor1stPath["SRV"]->GetCPUDescriptorHandleForHeapStart());
}

//2パス目
void RenderTarget::Init2ndPathRTVSRV(ID3D12Device * _dev) {
	D3D12_DESCRIPTOR_HEAP_DESC descHeap = {};
	descHeap.Flags			= D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	descHeap.NodeMask		= 0;
	descHeap.NumDescriptors = 1;

	//RTV用ヒープ
	descHeap.Type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	_dev->CreateDescriptorHeap(&descHeap, IID_PPV_ARGS(&heapFor2ndPath["RTV"]));

	//SRV用ヒープ
	descHeap.Type	= D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeap.Flags	= D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	_dev->CreateDescriptorHeap(&descHeap, IID_PPV_ARGS(&heapFor2ndPath["SRV"]));

	D3D12_HEAP_PROPERTIES hProp = {};
	hProp.Type					= D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_DEFAULT;
	hProp.CPUPageProperty		= D3D12_CPU_PAGE_PROPERTY::D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	hProp.MemoryPoolPreference	= D3D12_MEMORY_POOL::D3D12_MEMORY_POOL_UNKNOWN;
	hProp.CreationNodeMask		= 1;
	hProp.VisibleNodeMask		= 1;

	D3D12_RESOURCE_DESC rDesc = {};
	rDesc.Height			= WIN_HEIGHT;
	rDesc.Width				= WIN_WIDTH;
	rDesc.MipLevels			= 1;
	rDesc.DepthOrArraySize	= 1;
	rDesc.SampleDesc.Count	= 1;
	rDesc.Flags				= D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	rDesc.Dimension			= D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	rDesc.Layout			= D3D12_TEXTURE_LAYOUT::D3D12_TEXTURE_LAYOUT_UNKNOWN;
	rDesc.Format			= DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;

	//RTVSRVバッファ生成
	result = _dev->CreateCommittedResource(
		&hProp,
		D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
		&rDesc,
		D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_RENDER_TARGET,
		nullptr,
		IID_PPV_ARGS(&peraBuffer2nd)
	);

	//RTV生成
	_dev->CreateRenderTargetView(peraBuffer2nd, nullptr, heapFor2ndPath["RTV"]->GetCPUDescriptorHandleForHeapStart());

	//SRV生成
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format					= DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension			= D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels		= 1;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	_dev->CreateShaderResourceView(peraBuffer2nd, &srvDesc, heapFor2ndPath["SRV"]->GetCPUDescriptorHandleForHeapStart());
}

void RenderTarget::Set1stPathRTV(ID3D12GraphicsCommandList * list, ID3D12DescriptorHeap * depthHeap) {
	list->OMSetRenderTargets(1, &heapFor1stPath["RTV"]->GetCPUDescriptorHandleForHeapStart(), false, &depthHeap->GetCPUDescriptorHandleForHeapStart());
}

std::vector<ID3D12Resource*> RenderTarget::GetRenderTarget() {
	return renderTarget;
}

ID3D12Resource* RenderTarget::GetPeraRenderTarget() {
	return peraBuffer;
}

ID3D12Resource * RenderTarget::GetPeraRenderTarget2() {
	return peraBuffer2nd;
}

std::map<std::string, ID3D12DescriptorHeap*> RenderTarget::GetHeap()
{
	return heapFor1stPath;
}

std::map<std::string, ID3D12DescriptorHeap*> RenderTarget::GetHeap2nd() {
	return heapFor2ndPath;
}


RenderTarget::~RenderTarget()
{
}
