#pragma once
#include <tchar.h>
#include <vector>
#include <DirectXTex/DirectXTex.h>
#pragma comment(lib, "DirectXTex.lib")

//�e�N�X�`�����\�[�X
class TextureResource : public Result {
public:
	TextureResource();
	void Initialize(ID3D12Device* _dev, UINT sizeWidth, UINT sizeHeight);//������
	void WriteToTextureBuffer(const DirectX::Image* _data, std::vector<bool> textureFlag);//�e�N�X�`���o�b�t�@�֏�������
	ID3D12Resource* GetTextureBuffer();
	~TextureResource();
private:
	D3D12_RESOURCE_DESC texResourceDesc = {};
	D3D12_HEAP_PROPERTIES hprop = {};
	ID3D12Resource* textureBuffer;
	D3D12_BOX box = {};
};

