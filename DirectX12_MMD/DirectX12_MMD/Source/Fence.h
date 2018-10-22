#pragma once

//ƒtƒFƒ“ƒX
class Fence : public Result {
public:
	Fence();
	void InitFence(ID3D12Device* _dev);
	ID3D12Fence* GetFence();
	UINT64 GetFenceValue(bool countFlag = false);
	~Fence();
private:
	ID3D12Fence * fence;
	UINT64 fenceValue;
};

