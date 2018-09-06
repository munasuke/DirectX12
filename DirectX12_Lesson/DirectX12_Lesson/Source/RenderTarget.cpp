#include "Result.h"
#include "RenderTarget.h"
#include "SwapChain.h"
#include "Descriptor.h"


RenderTarget::RenderTarget()
{
}

void RenderTarget:: InitRenderTarget(UINT _bufferCnt, ID3D12Device* _dev, IDXGISwapChain3* _swapChain, CD3DX12_CPU_DESCRIPTOR_HANDLE _descriptorHandle, UINT _descriptorSize)
{
	renderNum = _bufferCnt;
	//�����_�[�^�[�Q�b�g�̐��Ԃ�m��
	renderTarget.resize(renderNum);

	for (UINT i = 0; i < renderNum; ++i) {
		//�X���b�v�`�F�C������L�����o�X���擾
		result = _swapChain->GetBuffer(i, IID_PPV_ARGS(&renderTarget[i]));
		//�L�����o�X�ƃr���[��R�Â�
		_dev->CreateRenderTargetView(renderTarget[i], nullptr, _descriptorHandle);
		//�L�����o�X�ƃr���[�̂Ԃ񂾂����̂Ƃ���܂ŃI�t�Z�b�g
		_descriptorHandle.Offset(_descriptorSize);
	}
}


RenderTarget::~RenderTarget()
{
}
