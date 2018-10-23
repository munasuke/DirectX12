#include "ImageLoader.h"
#include <locale>
#include <codecvt>

using namespace DirectX;

ImageLoader::ImageLoader() {
	//拡張子でロード関数を変更
	//bmp, png, jpg
	loadFuncTbl[L"bmp"] = loadFuncTbl[L"png"] = loadFuncTbl[L"jpg"] =
		[](const std::wstring& path, TexMetadata* meta, ScratchImage& img) -> HRESULT {
		return LoadFromWICFile(path.c_str(), 0, meta, img); };

	//tga
	loadFuncTbl[L"tga"] =
		[](const std::wstring& path, TexMetadata* meta, ScratchImage& img) -> HRESULT {
		return LoadFromTGAFile(path.c_str(), meta, img); };

	//dds
	loadFuncTbl[L"dds"] =
		[](const std::wstring& path, TexMetadata* meta, ScratchImage& img) -> HRESULT {
		return LoadFromDDSFile(path.c_str(), 0, meta, img); };
}

int ImageLoader::Load(const std::string path) {
	//StringをWStringに変換
	std::wstring wstr = ConvertStringToWString(path);

	//拡張子を判定
	auto index = wstr.rfind('.');
	auto filePath = wstr.substr(index + 1);

	//画像読み込み
	auto result = loadFuncTbl[filePath](wstr, &metaData, image);

	imageRect.emplace_back(XMINT2(metaData.width, metaData.height));
	imageData.emplace_back(image);

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

std::vector<DirectX::XMINT2> ImageLoader::GetImageRect()
{
	return imageRect;
}

std::vector<DirectX::ScratchImage> ImageLoader::GetImageData()
{
	return imageData;
}

ImageLoader::~ImageLoader() {
}

std::wstring ImageLoader::ConvertStringToWString(std::string str) {
	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> cv;
	return cv.from_bytes(str);
}
