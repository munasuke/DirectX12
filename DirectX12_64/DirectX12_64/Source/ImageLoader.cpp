#include "ImageLoader.h"
#include <locale>
#include <codecvt>

using namespace DirectX;

ImageLoader::ImageLoader() {
	//拡張子で関数変える
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

int ImageLoader::Load(const std::string path) {
	//string �� wstring
	std::wstring wstrPath = ConvertStringToWString(path);
	//�g���q�𔻒�
	std::wstring filePath = wstrPath;
	auto index = filePath.rfind('.');
	filePath = filePath.substr(index + 1);

	auto result = loadFuncTbl[filePath](wstrPath, &metaData, image);
	metaDataArray.emplace_back(metaData);
	imageArray.emplace_back(image.GetPixels());

	return 0;
}

DirectX::TexMetadata ImageLoader::GetMetaData()
{
	return metaData;
}

std::vector<DirectX::TexMetadata> ImageLoader::GetMetaArray() {
	return metaDataArray;
}

uint8_t * ImageLoader::GetScratchImage() {
	auto imgPixels = image.GetPixels();
	return imgPixels;
}

std::vector<uint8_t*> ImageLoader::GetScratchImageArray() {
	return imageArray;
}

ImageRect ImageLoader::GetImageRect() {
	return imageRec;
}

ImageLoader::~ImageLoader() {
}

std::wstring ImageLoader::ConvertStringToWString(std::string str) {
	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> cv;
	return cv.from_bytes(str);
}
