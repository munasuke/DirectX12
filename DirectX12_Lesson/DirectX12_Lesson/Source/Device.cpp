#include "Result.h"
#include "Device.h"
#include <d3d12.h>
#include <dxgi1_6.h>

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

namespace{
	//フューチャーレベル
	D3D_FEATURE_LEVEL levels[] = {
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0
	};
	D3D_FEATURE_LEVEL level = {};
}

Device::Device() : dev(nullptr)
{
	//エラーを出力に表示させる
#ifdef _DEBUG
	ID3D12Debug *debug	= nullptr;
	result				= D3D12GetDebugInterface(IID_PPV_ARGS(&debug));
	if (FAILED(result))
		int i = 0;
	debug->EnableDebugLayer();
	Release(debug);
	debug = nullptr;
#endif
	//マシンにあったバージョンを選ぶ
	for (auto i : levels){
		result = D3D12CreateDevice(nullptr, i, IID_PPV_ARGS(&dev));
		if (result == S_OK) {
			level = i;
			break;
		}
	}
}

ID3D12Device * Device::GetDevice() {
	return dev;
}


Device::~Device() {
	Release(dev);
}
