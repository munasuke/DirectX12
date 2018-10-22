#pragma once
#include <vector>
#include <dxgi1_4.h>

class SwapChain;
class Descriptor;

class RenderTarget : public Result
{
public:
	RenderTarget();

	//èâä˙âª
	void InitRenderTarget(UINT _bufferCnt, ID3D12Device* _dev, IDXGISwapChain3* _swapChain, CD3DX12_CPU_DESCRIPTOR_HANDLE _descriptorHandle, UINT _descriptorSize);
	std::vector<ID3D12Resource*> GetRenderTarget();

	~RenderTarget();
private:
	std::vector<ID3D12Resource*> renderTarget;
	UINT renderNum;

};

