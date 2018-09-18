#pragma once
#include <d3d12.h>

//�R�}���h����
class Command : public Result {
public:
	Command();

	void InitCommand(ID3D12Device* _dev);//������

	void Execute();

	ID3D12CommandAllocator*		GetCommandAllocator();	//�R�}���h�A���P�[�^��Ԃ�
	ID3D12CommandQueue*			GetCommandQueue();		//�R�}���h�L���[��Ԃ�
	ID3D12GraphicsCommandList*	GetCommandList();		//�R�}���h���X�g��Ԃ�

	~Command();
private:
	ID3D12CommandAllocator*		commandAllocator;	//�R�}���h�A���P�[�^
	ID3D12CommandQueue*			commandQueue;		//�R�}���h�L���[
	ID3D12GraphicsCommandList*	commandList;		//�R�}���h���X�g
};

