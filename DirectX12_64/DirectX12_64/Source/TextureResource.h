#pragma once
#include <tchar.h>
#include <vector>

//�e�N�X�`�����\�[�X
class TextureResource : public Result {
public:
	TextureResource();
	void Initialize(ID3D12Device* _dev);//������
	void WriteToTextureBuffer(std::vector<CHAR> _data);//�e�N�X�`���o�b�t�@�֏�������
	ID3D12Resource* GetTextureBuffer();
	~TextureResource();
private:
	D3D12_RESOURCE_DESC texResourceDesc = {};
	D3D12_HEAP_PROPERTIES hprop = {};
	ID3D12Resource* textureBuffer;
	D3D12_BOX box = {};
};

