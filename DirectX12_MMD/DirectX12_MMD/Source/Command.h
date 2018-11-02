#pragma once
#include <d3d12.h>

//コマンド周り
class Command : public Result {
public:
	Command();

	void InitCommand(ID3D12Device* _dev);//初期化

	void Execute();

	void CreateModelDrawBundle(ID3D12Device* _dev, ID3D12PipelineState* pip);

	ID3D12CommandAllocator*		GetCommandAllocator();	//コマンドアロケータを返す
	ID3D12CommandQueue*			GetCommandQueue();		//コマンドキューを返す
	ID3D12GraphicsCommandList*	GetCommandList();		//コマンドリストを返す

	ID3D12CommandAllocator*		GetBundleAllocator();	//コマンドアロケータを返す
	ID3D12GraphicsCommandList*	GetBundleCommandList();		//コマンドリストを返す

	~Command();
private:
	ID3D12CommandAllocator * commandAllocator;	//コマンドアロケータ
	ID3D12CommandQueue*			commandQueue;		//コマンドキュー
	ID3D12GraphicsCommandList*	commandList;		//コマンドリスト

	ID3D12CommandAllocator* bundleAllocator;
	ID3D12GraphicsCommandList* bundleCommandList;
};

