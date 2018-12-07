#pragma once
#include <vector>
#include <dxgi1_4.h>
#include <map>
#include <string>

class Window;
class SwapChain;
class Descriptor;

class RenderTarget : public Result
{
public:
	RenderTarget();

	//������
	void InitRenderTarget(UINT _bufferCnt, ID3D12Device* _dev, IDXGISwapChain3* _swapChain, CD3DX12_CPU_DESCRIPTOR_HANDLE _descriptorHandle, UINT _descriptorSize);

	//1�p�X�ڂɎg�������_�����O�o�b�t�@
	void Init1stPathRTVSRV(ID3D12Device* _dev);
	void InitBloomRTVSRV(ID3D12Device* _dev);
	//2�p�X��
	void Init2ndPathRTVSRV(ID3D12Device* _dev);
	void Set1stPathRTV(ID3D12GraphicsCommandList* list, ID3D12DescriptorHeap* depthHeap);

	std::vector<ID3D12Resource*> GetRenderTarget();
	ID3D12Resource* GetPeraRenderTarget();
	ID3D12Resource* GetBloomRenderTarget();
	ID3D12Resource* GetPeraRenderTarget2();
	std::map<std::string, ID3D12DescriptorHeap*> GetHeap();
	std::map<std::string, ID3D12DescriptorHeap*> GetHeapBloom();
	std::map<std::string, ID3D12DescriptorHeap*> GetHeap2nd();

	~RenderTarget();
private:
	std::vector<ID3D12Resource*> renderTarget;
	UINT renderNum;

	std::map<std::string, ID3D12DescriptorHeap*> heapFor1stPath;
	ID3D12Resource* peraBuffer;
	std::map<std::string, ID3D12DescriptorHeap*> heapFor2ndPath;
	ID3D12Resource* peraBuffer2nd;

	std::map<std::string, ID3D12DescriptorHeap*> heapForBloom;
	ID3D12Resource* bloomBuffer;
};

