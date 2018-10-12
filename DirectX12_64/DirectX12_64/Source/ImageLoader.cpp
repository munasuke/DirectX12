#include "ImageLoader.h"

using namespace DirectX;

ImageLoader::ImageLoader() {
	//�g���q�Ń��[�h�֐��𕪂���
	//bmp, png, jpg�p
	loadFuncTbl[L"bmp"] = loadFuncTbl[L"png"] = loadFuncTbl[L"jpg"] =
		[](const std::wstring& path, TexMetadata* meta, ScratchImage& img) -> HRESULT {
		return LoadFromWICFile(path.c_str(), 0, meta, img); };

	//tga�p
	loadFuncTbl[L"tga"] = 
		[](const std::wstring& path, TexMetadata* meta, ScratchImage& img) -> HRESULT {
		return LoadFromTGAFile(path.c_str(), meta, img); };

	//dds�p
	loadFuncTbl[L"dds"] =
		[](const std::wstring& path, TexMetadata* meta, ScratchImage& img) -> HRESULT {
		return LoadFromDDSFile(path.c_str(), 0, meta, img); };
}

int ImageLoader::Load(const std::wstring path) {
	//�g���q�𔻒�
	std::wstring filePath = path;
	auto index = filePath.rfind('.');
	filePath = filePath.substr(index + 1);

	TexMetadata metaData;
	ScratchImage img;//�摜�̖{�̂�����
	auto result = loadFuncTbl[filePath](path, &metaData, img);
	image = img.GetImage(0, 0, 0);
	
	return 0;
}

const DirectX::Image * ImageLoader::GetImage() {
	return image;
}


ImageLoader::~ImageLoader() {
}
