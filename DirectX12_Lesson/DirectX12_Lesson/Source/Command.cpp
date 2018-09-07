#include "Result.h"
#include "Command.h"

Command::Command() : 
	commandAllocator(nullptr), 
	commandQueue(nullptr), 
	commandList(nullptr)
{
}

void Command::InitCommand(ID3D12Device* _dev) {
	//コマンドアロケータの作成
	result = _dev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator));
	//コマンドリストの作成
	result = _dev->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator, nullptr, IID_PPV_ARGS(&commandList));
	//コマンドキューの作成
	D3D12_COMMAND_QUEUE_DESC desc	= {};
	desc.Flags						= D3D12_COMMAND_QUEUE_FLAG_NONE;
	desc.NodeMask					= 0;
	desc.Priority					= 0;
	desc.Type						= D3D12_COMMAND_LIST_TYPE_DIRECT;
	result = _dev->CreateCommandQueue(&desc, IID_PPV_ARGS(&commandQueue));
}

void Command::Execute() {
	ID3D12CommandList* commandLists[] = { commandList };
	commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);
}

ID3D12CommandAllocator * Command::GetCommandAllocator() {
	return commandAllocator;
}

ID3D12CommandQueue * Command::GetCommandQueue() {
	return commandQueue;
}

ID3D12GraphicsCommandList * Command::GetCommandList() {
	return commandList;
}


Command::~Command() {
	//解放処理
	Release(commandQueue);
	Release(commandList);
	Release(commandAllocator);
}
