#include "ImageLoader.h"

using namespace DirectX;

ImageLoader::ImageLoader() {
	//拡張子でロード関数を分ける
	//bmp, png, jpg用
	loadFuncTbl[L"bmp"] = loadFuncTbl[L"png"] = loadFuncTbl[L"jpg"] =
		[](const std::wstring& path, TexMetadata* meta, ScratchImage& img) -> HRESULT {
		return LoadFromWICFile(path.c_str(), 0, meta, img); };

	//tga用
	loadFuncTbl[L"tga"] = 
		[](const std::wstring& path, TexMetadata* meta, ScratchImage& img) -> HRESULT {
		return LoadFromTGAFile(path.c_str(), meta, img); };

	//dds用
	loadFuncTbl[L"dds"] =
		[](const std::wstring& path, TexMetadata* meta, ScratchImage& img) -> HRESULT {
		return LoadFromDDSFile(path.c_str(), 0, meta, img); };
}

int ImageLoader::Load(const std::wstring path) {
	//拡張子を判定
	std::wstring filePath = path;
	auto index = filePath.rfind('.');
	filePath = filePath.substr(index + 1);

	auto result = loadFuncTbl[filePath](path, &metaData, image);

	return 0;
}

DirectX::TexMetadata ImageLoader::GetMetaData()
{
	return metaData;
}

uint8_t * ImageLoader::GetScratchImage() {
	auto imgPixels = image.GetPixels();
	return imgPixels;
}

ImageRect ImageLoader::GetImageRect() {
	return imageRec;
}


ImageLoader::~ImageLoader() {
}

std::wstring ImageLoader::ConvertStringToWString(std::string str) {


	return std::wstring();
}
