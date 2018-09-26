#pragma once
#include "Matrix.h"
#include <memory>

//�萔�o�b�t�@
class ConstantBuffer : public Result{
public:
	ConstantBuffer();

	/*!
	* @fn
	* �������֐�
	* @brief	�R���X�^���g�o�b�t�@�̏���������B
	* @param	(ID3D12Device* _dev) �f�o�C�X
	*/
	void Initialize(ID3D12Device* _dev, ID3D12DescriptorHeap* _heap);
	~ConstantBuffer();

	/*!
	* @fn
	* �s��ɍX�V��������֐�
	* @brief	wvp�s����X�V����B
	* @detail	���[���h�s��A�r���[�s��A�v���W�F�N�V�����s����X�V����B
	*/
	void UpDataWVP();

	/*!
	* @fn
	* �f�X�N���v�^�ɃR���X�^���g�o�b�t�@���Z�b�g����֐�
	* @brief	�f�X�N���v�^�փR���X�^���g�o�b�t�@���Z�b�g����B
	* @param	(ID3D12GraphicsCommandList* _list) �R�}���h���X�g
	*/
	void SetDescriptor(ID3D12GraphicsCommandList* _list, int _index, ID3D12DescriptorHeap* _heap, ID3D12Device* _dev);
private:
	Matrixs mt;//��ԍs��
	UINT* data;


	ID3D12Resource* constantBuffer;//�萔�o�b�t�@�p
	ID3D12DescriptorHeap* cbvDescHeap;

	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc = {};
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	CD3DX12_HEAP_PROPERTIES heapProperties = {};

	D3D12_RESOURCE_DESC cbvResDesc = {};
};

