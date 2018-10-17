#include "Result.h"
#include "Fence.h"



Fence::Fence() : fence(nullptr), fenceValue(0) {
}

void Fence::InitFence(ID3D12Device * _dev) {
	result = _dev->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
}

ID3D12Fence * Fence::GetFence() {
	return fence;
}

UINT64 Fence::GetFenceValue(bool countFlag) {
	return countFlag ? ++fenceValue : fenceValue;
}


Fence::~Fence() {
	ReleaseP(fence);
}
