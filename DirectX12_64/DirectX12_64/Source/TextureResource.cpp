#include "Result.h"
#include "TextureResource.h"



TextureResource::TextureResource()
{
}

void TextureResource::Initialize(ID3D12Device * _dev, DirectX::TexMetadata metaData)
{
	//�e�N�X�`�����o�b�t�@�[�쐬
	CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	texResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	texResourceDesc.Width = metaData.width;
	texResourceDesc.Height = metaData.height;
	texResourceDesc.DepthOrArraySize = 1;
	texResourceDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
	texResourceDesc.SampleDesc.Count = 1;
	texResourceDesc.Flags = D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_NONE;
	texResourceDesc.Layout = D3D12_TEXTURE_LAYOUT::D3D12_TEXTURE_LAYOUT_UNKNOWN;

	hprop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY::D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	hprop.CreationNodeMask = 1;
	hprop.MemoryPoolPreference = D3D12_MEMORY_POOL::D3D12_MEMORY_POOL_L0;
	hprop.Type = D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD;
	hprop.VisibleNodeMask = 1;

	result = _dev->CreateCommittedResource(
		&hprop,
		D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
		&texResourceDesc,
		D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&textureBuffer));


	box.top = 0;
	box.bottom = metaData.height;
	box.left = 0;
	box.right = metaData.width;
	box.front = 0;
	box.back = 1;
}

void TextureResource::WriteToTextureBuffer(std::vector<CHAR> _data, std::vector<bool> textureFlag) {
	//for (int i = 0; i < textureFlag.size(); i++) {
	//	if (textureFlag[i]) {
	//		result = textureBuffer->WriteToSubresource(0, &box, _data.data(), box.right * 4, box.bottom * 4);
	//	}
	//}
}

void TextureResource::WriteToTextureBuffer(DirectX::TexMetadata metaData, uint8_t* img, std::vector<bool> textureFlag) {
	//for (auto& texBuff : textureBuffer) {
	//	for (UINT i = 0; i < textureFlag.size(); i++) {
	//		if (textureFlag[i]) {
	//			result = texBuff->WriteToSubresource(0, nullptr, img, meta.width * 4, meta.height * 4);
	//			break;
	//		}
	//	}
	//}

	//for (auto flag : textureFlag) {
	//	if (flag) {
	//		result = textureBuffer->WriteToSubresource(0, nullptr, img, metaData.width * 4, metaData.height * 4);
	//	}
	//}

	for (int i = 0; i < textureFlag.size(); i++) {
		if (textureFlag[i]) {
			//�}�e���A���Ƀe�N�X�`���������ꍇ�̂ݏ�������
			result = textureBuffer->WriteToSubresource(0, nullptr, img, metaData.width * 4, metaData.height * 4);
		}
	}
}

ID3D12Resource* TextureResource::GetTextureBuffer() {
	return textureBuffer;
}


TextureResource::~TextureResource()
{
	ReleaseP(textureBuffer);
}
