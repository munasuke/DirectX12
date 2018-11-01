#pragma once
#include "d3dx12.h"
#include <map>
#include <vector>
#include <string>
#include <functional>
#include <memory>
#include <DirectXMath.h>
#include <DirectXTex/DirectXTex.h>
#pragma comment(lib, "DirectXTex.lib")

class ImageLoader {
public:
	ImageLoader(ID3D12Device* dev);

	int Initialize(int materialSize);
	int Load(const std::string path, int materialsize, int materialIndex);
	DirectX::TexMetadata GetMetaData();
	std::vector<ID3D12Resource*> GetTextureBuffer();
	std::vector<ID3D12Resource*> GetSphBuffer();
	std::vector<ID3D12Resource*> GetSpaBuffer();
	std::map<int, ID3D12Resource*> GetBufferMap();

	~ImageLoader();
private:
	std::wstring ConvertStringToWString(std::string str);

	ID3D12Device* dev;

	DirectX::TexMetadata	metaData;
	DirectX::ScratchImage	image;

	//�g���q�Ń��[�h�֐��𕪂���
	std::map
		<std::wstring,
		std::function<HRESULT(const std::wstring& path,
			DirectX::TexMetadata* meta,
			DirectX::ScratchImage& img)>> loadFuncTbl;

	//�ʏ�e�N�X�`��
	std::vector<ID3D12Resource*> textureBuffer;
	//��Z�e�N�X�`��
	std::vector<ID3D12Resource*> sphBuffer;
	//���Z�e�N�X�`��
	std::vector<ID3D12Resource*> spaBuffer;

	std::map<int, ID3D12Resource*> bufferMap;
};
