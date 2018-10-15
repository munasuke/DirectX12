#include "Result.h"
#include "ShaderResourceView.h"



ShaderResourceView::ShaderResourceView() : textureHeap(nullptr)
{
}

void ShaderResourceView::Initialize(ID3D12Device* _dev, std::vector<ID3D12Resource*> _texBuffer, UINT size) {
	//シェーダリソースビューの作成
	heapDesc.NumDescriptors = 2 + size;//領域確保
	heapDesc.Type			= D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	heapDesc.Flags			= D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	result = _dev->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&textureHeap));

	srvDesc.Format					= DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension			= D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels		= 1;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	for (auto texBuff : _texBuffer) {
		_dev->CreateShaderResourceView(texBuff, &srvDesc, textureHeap->GetCPUDescriptorHandleForHeapStart());
	}
}

ID3D12DescriptorHeap * ShaderResourceView::GetTextureHeap() {
	return textureHeap;
}

D3D12_DESCRIPTOR_HEAP_DESC ShaderResourceView::GetDescriptorHeapDesc() {
	return heapDesc;
}

ID3D12DescriptorHeap * const * ShaderResourceView::GetTextureHeap2() {
	return (ID3D12DescriptorHeap* const*)&textureHeap;
}


ShaderResourceView::~ShaderResourceView() {
	ReleaseP(textureHeap);
}
