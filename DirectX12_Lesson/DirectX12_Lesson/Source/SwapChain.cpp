#include "Result.h"
#include "SwapChain.h"
#include "Window.h"
#include "Command.h"



SwapChain::SwapChain() : factory(nullptr), swapChain(nullptr)
{
}

void SwapChain::InitSwapChain(ID3D12CommandQueue* _commandQueue, HWND _hwnd) {
	result = CreateDXGIFactory(IID_PPV_ARGS(&factory));

	swapChainDesc.Width					= WIN_WIDTH;
	swapChainDesc.Height				= WIN_HEIGHT;
	swapChainDesc.Format				= DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.Stereo				= 0;
	swapChainDesc.SampleDesc.Count		= 1;
	swapChainDesc.SampleDesc.Quality	= 0;
	swapChainDesc.BufferUsage			= DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount			= 2;
	swapChainDesc.Scaling				= DXGI_SCALING_STRETCH;
	swapChainDesc.SwapEffect			= DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.AlphaMode				= DXGI_ALPHA_MODE_UNSPECIFIED;

	result = factory->CreateSwapChainForHwnd(_commandQueue,
		_hwnd,
		&swapChainDesc,
		nullptr,
		nullptr,
		(IDXGISwapChain1**)(&swapChain));
}

DXGI_SWAP_CHAIN_DESC1 SwapChain::GetSwapChainDesc()
{
	return swapChainDesc;
}

IDXGISwapChain3 * SwapChain::GetSwapChain()
{
	return swapChain;
}


SwapChain::~SwapChain() {
	Release(swapChain);
	Release(factory);
}
