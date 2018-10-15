#include "Result.h"
#include "TextureResource.h"



TextureResource::TextureResource()
{
}

//void TextureResource::Initialize(ID3D12Device * _dev, const UINT textureNum, const UINT sizeWidth, const UINT sizeHeight)
//{
//	//�e�N�X�`�����\�[�X�̍쐬
//	CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
//	texResourceDesc.Dimension			= D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_TEXTURE2D;
//	texResourceDesc.Width				= sizeWidth;
//	texResourceDesc.Height				= sizeHeight;
//	texResourceDesc.DepthOrArraySize	= 1;
//	texResourceDesc.Format				= DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
//	texResourceDesc.SampleDesc.Count	= 1;
//	texResourceDesc.Flags				= D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_NONE;
//	texResourceDesc.Layout				= D3D12_TEXTURE_LAYOUT::D3D12_TEXTURE_LAYOUT_UNKNOWN;
//
//	hprop.Type					= D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_CUSTOM;
//	hprop.CPUPageProperty		= D3D12_CPU_PAGE_PROPERTY::D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
//	hprop.MemoryPoolPreference	= D3D12_MEMORY_POOL::D3D12_MEMORY_POOL_L0;
//	hprop.CreationNodeMask		= 1;
//	hprop.VisibleNodeMask		= 1;
//
//	//�e�N�X�`�����m��
//	textureBuffer.resize(textureNum);
//	//�e�N�X�`�����o�b�t�@�[�쐬
//	for (auto& texBuff : textureBuffer) {
//		result = _dev->CreateCommittedResource(
//			&hprop,
//			D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
//			&texResourceDesc,
//			D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ,
//			nullptr,
//			IID_PPV_ARGS(&texBuff));
//	}
//
//	box.top		= 0;
//	box.bottom	= sizeHeight;
//	box.left	= 0;
//	box.right	= sizeWidth;
//	box.front	= 0;
//	box.back	= 1;
//}

void TextureResource::Initialize(ID3D12Device * _dev, UINT sizeWidth, UINT sizeHeight)
{
	//�e�N�X�`�����\�[�X�̍쐬
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

	//�e�N�X�`�����o�b�t�@�[�쐬
	for (auto& meta : metaData) {
		//�e�N�X�`�����\�[�X�̍쐬
		CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		texResourceDesc.Dimension			= D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		texResourceDesc.Width				= meta.width;
		texResourceDesc.Height				= meta.height;
		texResourceDesc.DepthOrArraySize	= 1;
		texResourceDesc.Format				= DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
		texResourceDesc.SampleDesc.Count	= 1;
		texResourceDesc.Flags				= D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_NONE;
		texResourceDesc.Layout				= D3D12_TEXTURE_LAYOUT::D3D12_TEXTURE_LAYOUT_UNKNOWN;

		ID3D12Resource* texBuff;
		result = _dev->CreateCommittedResource(
			&hprop,
			D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
			&texResourceDesc,
			D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&texBuff));

		textureBuffer.emplace_back(texBuff);

		box.top		= 0;
		box.bottom	= meta.height;
		box.left	= 0;
		box.right	= meta.width;
		box.front	= 0;
		box.back	= 1;
	}

}

void TextureResource::WriteToTextureBuffer(std::vector<CHAR> _data, std::vector<bool> textureFlag) {
	//for (int i = 0; i < textureFlag.size(); i++) {
	//	if (textureFlag[i]) {
	//		result = textureBuffer->WriteToSubresource(0, &box, _data.data(), box.right * 4, box.bottom * 4);
	//	}
	//}
}

void TextureResource::WriteToTextureBuffer(std::vector<DirectX::TexMetadata> metaData, std::vector<uint8_t*> img, std::vector<bool> textureFlag) {
	//for (auto& texBuff : textureBuffer) {
	//	for (UINT i = 0; i < textureFlag.size(); i++) {
	//		if (textureFlag[i]) {
	//			result = texBuff->WriteToSubresource(0, nullptr, img, meta.width * 4, meta.height * 4);
	//			break;
	//		}
	//	}
	//}
	UINT i = 0;
	for (auto flag : textureFlag) {
		if (flag) {
			result = textureBuffer[i]->WriteToSubresource(0, nullptr, img[i], metaData[i].width * 4, metaData[i].height * 4);
			++i;
		}
	}

	//for (int i = 0; i < textureFlag.size(); i++) {
	//	if (textureFlag[i]) {
	//		D3D12_BOX box = {};
	//		//�}�e���A���Ƀe�N�X�`���������ꍇ�̂ݏ�������
	//		result = textureBuffer->WriteToSubresource(0, &box, img, meta.width * 4, meta.height * 4);
	//	}
	//}
}

std::vector<ID3D12Resource*> TextureResource::GetTextureBuffer() {
	return textureBuffer;
}


TextureResource::~TextureResource()
{
	for (auto itr = textureBuffer.begin(); itr != textureBuffer.end(); itr++){
		ReleaseP(*itr);
	}
}
