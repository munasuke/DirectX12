#include "Result.h"
#include "Command.h"

Command::Command(ID3D12Device* _dev) : 
	commandAllocator(nullptr), 
	commandQueue(nullptr), 
	commandList(nullptr)
{
	//�R�}���h�A���P�[�^�̍쐬
	result = _dev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator));
	//�R�}���h���X�g�̍쐬
	result = _dev->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator, nullptr, IID_PPV_ARGS(&commandList));
	//�R�}���h�L���[�̍쐬
	D3D12_COMMAND_QUEUE_DESC desc	= {};
	desc.Flags						= D3D12_COMMAND_QUEUE_FLAG_NONE;
	desc.NodeMask					= 0;
	desc.Priority					= 0;
	desc.Type						= D3D12_COMMAND_LIST_TYPE_DIRECT;
	result = _dev->CreateCommandQueue(&desc, IID_PPV_ARGS(&commandQueue));
}


Command::~Command() {
	//�������
	Release(commandQueue);
	Release(commandList);
	Release(commandAllocator);
}
