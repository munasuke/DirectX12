#include "Result.h"
#include "TextureResource.h"



TextureResource::TextureResource() : textureBuffer(nullptr)
{
}

void TextureResource::Initialize(ID3D12Device * _dev, UINT sizeWidth, UINT sizeHeight)
{
	//テクスチャリソースの作成
	CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	texResourceDesc.Dimension			= D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	texResourceDesc.Width				= sizeWidth;
	texResourceDesc.Height				= sizeHeight;
	texResourceDesc.DepthOrArraySize	= 1;
	texResourceDesc.Format				= DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
	texResourceDesc.SampleDesc.Count	= 1;
	texResourceDesc.Flags				= D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_NONE;
	texResourceDesc.Layout				= D3D12_TEXTURE_LAYOUT::D3D12_TEXTURE_LAYOUT_UNKNOWN;

	hprop.Type					= D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_CUSTOM;
	hprop.CPUPageProperty		= D3D12_CPU_PAGE_PROPERTY::D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	hprop.MemoryPoolPreference	= D3D12_MEMORY_POOL::D3D12_MEMORY_POOL_L0;
	hprop.CreationNodeMask		= 1;
	hprop.VisibleNodeMask		= 1;

	result = _dev->CreateCommittedResource(
		&hprop,
		D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
		&texResourceDesc,
		D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&textureBuffer));

	box.top		= 0;
	box.bottom	= sizeHeight;
	box.left	= 0;
	box.right	= sizeWidth;
	box.front	= 0;
	box.back	= 1;
}

void TextureResource::WriteToTextureBuffer(std::vector<CHAR> _data, std::vector<BOOL> textureFlag) {
	auto flag = textureFlag.data();
	for (auto itr = textureFlag.begin(); itr != textureFlag.end(); itr++) {
		if (flag++) {
			//マテリアルにテクスチャがある場合のみ書き込む
			result = textureBuffer->WriteToSubresource(0, &box, _data.data(), box.right * 4, box.bottom * 4);
		}
	}
}

ID3D12Resource * TextureResource::GetTextureBuffer() {
	return textureBuffer;
}


TextureResource::~TextureResource()
{
	ReleaseP(textureBuffer);
}
