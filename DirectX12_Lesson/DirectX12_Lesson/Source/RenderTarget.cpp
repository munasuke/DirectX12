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


RenderTarget::~RenderTarget()
{
}
