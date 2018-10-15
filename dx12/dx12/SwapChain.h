#pragma once

#include <dxgi1_4.h>
#include <memory>

class SwapChain : public Result{
public:
	SwapChain();

	void InitSwapChain(ID3D12CommandQueue* _commandQueue, HWND _hwnd);//èâä˙âª
	DXGI_SWAP_CHAIN_DESC1 GetSwapChainDesc();
	IDXGISwapChain3* GetSwapChain();

	~SwapChain();
private:
	IDXGIFactory4* factory;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	IDXGISwapChain3* swapChain;
};

