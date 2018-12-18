#include "Device.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#include <vector>
#include <string>

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

namespace {
	//フューチャーレベル
	D3D_FEATURE_LEVEL levels[] = {
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0
	};
	D3D_FEATURE_LEVEL level = {};
}

Device::Device(IDXGIFactory4 * factory) : dev(nullptr)
{
	//エラーを出力に表示させる
#ifdef _DEBUG
	ID3D12Debug *debug = nullptr;
	auto result = D3D12GetDebugInterface(IID_PPV_ARGS(&debug));
	if (FAILED(result))
		int i = 0;
	debug->EnableDebugLayer();
	debug->Release();
	debug = nullptr;
#endif

	//グラボを選ぶ
	std::vector<IDXGIAdapter*> adapters;
	IDXGIAdapter* adapter = nullptr;
	for (int i = 0; factory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND; ++i) {
		adapters.push_back(adapter);
	}

	//NVIDIAを探す
	for (auto adpt : adapters) {
		DXGI_ADAPTER_DESC adDesc = {};
		adpt->GetDesc(&adDesc);
		std::wstring wstr = adDesc.Description;
		if (wstr.find(L"NVIDIA") != std::string::npos) {
			adapter = adpt;
			break;
		}
	}

	//マシンにあったバージョンを選ぶ
	for (auto i : levels) {
		if (D3D12CreateDevice(adapter, i, IID_PPV_ARGS(&dev)) == S_OK) {
			level = i;
			break;
		}
	}
}

void Device::Initialize(IDXGIFactory4 * factory) {
}

ID3D12Device * Device::GetDevice() {
	return dev;
}


Device::~Device() {
	dev->Release();
}
