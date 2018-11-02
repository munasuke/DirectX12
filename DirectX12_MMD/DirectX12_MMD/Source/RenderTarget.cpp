#include "Result.h"
#include "RenderTarget.h"
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

void RenderTarget::Init1stPathRTVSRV(ID3D12Device* _dev, ID3D12Resource* buff) {
	D3D12_DESCRIPTOR_HEAP_DESC descHeap = {};
	descHeap.Flags = D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	descHeap.NodeMask = 0;
	descHeap.NumDescriptors = 1;
	descHeap.Type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	_dev->CreateDescriptorHeap(&descHeap, IID_PPV_ARGS(&heapFor1stPath));


}

std::vector<ID3D12Resource*> RenderTarget::GetRenderTarget() {
	return renderTarget;
}


RenderTarget::~RenderTarget()
{
}
