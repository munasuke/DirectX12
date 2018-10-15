#pragma once
#include <tchar.h>
#include <vector>
#include <DirectXTex/DirectXTex.h>
#pragma comment(lib, "DirectXTex.lib")

//�e�N�X�`�����\�[�X
class TextureResource : public Result {
public:
	TextureResource();
	//void Initialize(ID3D12Device* _dev, const UINT textureNum, const UINT sizeWidth, const UINT sizeHeight);//������
	void Initialize(ID3D12Device* _dev, std::vector<DirectX::TexMetadata> metaData);//������
	void WriteToTextureBuffer(std::vector<CHAR> _data, std::vector<bool> textureFlag);//�e�N�X�`���o�b�t�@�֏�������
	void WriteToTextureBuffer(std::vector<DirectX::TexMetadata> metaData, std::vector<uint8_t*> img, std::vector<bool> textureFlag);//�e�N�X�`���o�b�t�@�֏�������
	std::vector<ID3D12Resource*> GetTextureBuffer();
	void Initialize(ID3D12Device* _dev, UINT sizeWidth, UINT sizeHeight);//������
	void WriteToTextureBuffer(const DirectX::Image* _data, std::vector<bool> textureFlag);//�e�N�X�`���o�b�t�@�֏�������
	ID3D12Resource* GetTextureBuffer();
	~TextureResource();
private:
	D3D12_RESOURCE_DESC texResourceDesc = {};
	D3D12_HEAP_PROPERTIES hprop = {};
	std::vector<ID3D12Resource*> textureBuffer;
	D3D12_BOX box = {};
};
