#pragma once
#include <d3d12.h>

//コマンド周り
class Command : public Result {
public:
	Command(ID3D12Device* _dev);
	~Command();
private:
	ID3D12CommandAllocator*		commandAllocator;//コマンドアロケータ
	ID3D12CommandQueue*			commandQueue;//コマンドキュー
	ID3D12GraphicsCommandList*	commandList;//コマンドリスト
};

