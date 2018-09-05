#pragma once
#include <d3d12.h>

//�R�}���h����
class Command : public Result {
public:
	Command(ID3D12Device* _dev);
	~Command();
private:
	ID3D12CommandAllocator*		commandAllocator;//�R�}���h�A���P�[�^
	ID3D12CommandQueue*			commandQueue;//�R�}���h�L���[
	ID3D12GraphicsCommandList*	commandList;//�R�}���h���X�g
};

