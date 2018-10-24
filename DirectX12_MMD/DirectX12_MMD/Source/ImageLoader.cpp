#include "ImageLoader.h"
#include "Device.h"
#include <locale>
#include <codecvt>

using namespace DirectX;

ImageLoader::ImageLoader(std::shared_ptr<Device> dev) : dev(dev){
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

	//テクスチャリソースの設定
	D3D12_RESOURCE_DESC texResourceDesc = {};
	CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	texResourceDesc.Width				= metaData.width;
	texResourceDesc.Height				= metaData.height;
	texResourceDesc.Dimension			= D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	texResourceDesc.Format				= DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
	texResourceDesc.Flags				= D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_NONE;
	texResourceDesc.Layout				= D3D12_TEXTURE_LAYOUT::D3D12_TEXTURE_LAYOUT_UNKNOWN;
	texResourceDesc.DepthOrArraySize	= 1;
	texResourceDesc.SampleDesc.Count	= 1;

	//プロパティ
	D3D12_HEAP_PROPERTIES hprop = {};
	hprop.CPUPageProperty		= D3D12_CPU_PAGE_PROPERTY::D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	hprop.MemoryPoolPreference	= D3D12_MEMORY_POOL::D3D12_MEMORY_POOL_L0;
	hprop.Type					= D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_CUSTOM;
	hprop.CreationNodeMask		= 1;
	hprop.VisibleNodeMask		= 1;

	//バッファ生成
	ID3D12Resource* buffer = nullptr;
	result = dev.lock()->GetDevice()->CreateCommittedResource(
		&hprop,
		D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
		&texResourceDesc,
		D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&buffer));

	textureBuffer.emplace_back(buffer);

	imageRect.emplace_back(XMINT2(metaData.width, metaData.height));

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

std::vector<ID3D12Resource*> ImageLoader::GetTextureBuffer()
{
	return textureBuffer;
}

ImageLoader::~ImageLoader() {
}

std::wstring ImageLoader::ConvertStringToWString(std::string str) {
	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> cv;
	return cv.from_bytes(str);
}
