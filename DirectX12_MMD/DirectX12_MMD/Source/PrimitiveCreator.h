#pragma once
#include <memory>
#include <vector>
#include <DirectXMath.h>
#include "d3dx12.h"

class PrimitiveObject;
class Plane;
class Cone;

using namespace DirectX;

class PrimitiveManager {
public:
	PrimitiveManager(ID3D12Device* d);

	//�v���~�e�B�u�p�̃��C�A�E�g�A�V�F�[�_�A�p�C�v���C���̏�����
	void Initialize();

	//�v���~�e�B�u�p�̃p�C�v���C���̃Z�b�g�B�`��O�ɕK���Ă�
	void SetPrimitiveDrawMode(ID3D12GraphicsCommandList* list);

	//�`��
	void Draw(ID3D12GraphicsCommandList* list);

	void SetVertices(ID3D12GraphicsCommandList* list);

	/**
	* @brief ���ʃI�u�W�F�N�g�̐���
	* @param pos	���ʂ̍��W
	* @param width	���ʂ̕�
	* @param depth	���ʂ̉��s
	* @param color	���ʂ̐F
	*/
	std::shared_ptr<Plane> CreatePlane(const XMFLOAT3& pos, const float width, const float depth, const XMFLOAT4& color);

	/**
	* @brief �~���I�u�W�F�N�g�̐���
	* @param pos	�~���̍��W
	* @param div	�~���̕�����
	* @param r		�~���̔��a
	* @param height	�~���̍���
	* @param color	�~���̐F
	*/
	std::shared_ptr<Cone> CreateCone(const XMFLOAT3& pos, const unsigned int div, const float r, const float height, const XMFLOAT4& color);

	~PrimitiveManager();

private:
	ID3D12PipelineState* piplineState;
	ID3D12RootSignature* rs;
	ID3D12Resource* resource;
	std::vector<std::shared_ptr<PrimitiveObject>> obj;
	ID3D12Device* dev;
};

