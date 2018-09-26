#pragma once
#include "Matrix.h"
#include <memory>

//定数バッファ
class ConstantBuffer : public Result{
public:
	ConstantBuffer();

	/*!
	* @fn
	* 初期化関数
	* @brief	コンスタントバッファの初期化する。
	* @param	(ID3D12Device* _dev) デバイス
	*/
	void Initialize(ID3D12Device* _dev, ID3D12DescriptorHeap* _heap);
	~ConstantBuffer();

	/*!
	* @fn
	* 行列に更新をかける関数
	* @brief	wvp行列を更新する。
	* @detail	ワールド行列、ビュー行列、プロジェクション行列を更新する。
	*/
	void UpDataWVP();

	/*!
	* @fn
	* デスクリプタにコンスタントバッファをセットする関数
	* @brief	デスクリプタへコンスタントバッファをセットする。
	* @param	(ID3D12GraphicsCommandList* _list) コマンドリスト
	*/
	void SetDescriptor(ID3D12GraphicsCommandList* _list, int _index, ID3D12DescriptorHeap* _heap, ID3D12Device* _dev);
private:
	Matrixs mt;//空間行列
	UINT* data;


	ID3D12Resource* constantBuffer;//定数バッファ用
	ID3D12DescriptorHeap* cbvDescHeap;

	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc = {};
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	CD3DX12_HEAP_PROPERTIES heapProperties = {};

	D3D12_RESOURCE_DESC cbvResDesc = {};
};

