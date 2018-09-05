#pragma once

#include <dxgi1_4.h>

class SwapChain {
public:
	SwapChain();
	~SwapChain();
private:
	IDXGIFactory4* factory;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
};

