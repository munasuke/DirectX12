#include "ImageLoader.h"
#include <locale>
#include <codecvt>

using namespace DirectX;

ImageLoader::ImageLoader(ID3D12Device* dev) : dev(dev){
	//拡張子でロード関数を変更
	//bmp, png, jpg
	loadFuncTbl[L"bmp"] = loadFuncTbl[L"png"] = loadFuncTbl[L"jpg"] = loadFuncTbl[L"sph"] = loadFuncTbl[L"spa"] =
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

int ImageLoader::Load(const std::string path, int materialsize, int materialIndex) {
	textureBuffer.resize(materialsize);
	sphBuffer.resize(materialsize);
	spaBuffer.resize(materialsize);

	//テクスチャリソースの設定
	D3D12_RESOURCE_DESC texResourceDesc = {};
	CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	texResourceDesc.Width = 4;
	texResourceDesc.Height = 4;
	texResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	texResourceDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
	texResourceDesc.Flags = D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_NONE;
	texResourceDesc.Layout = D3D12_TEXTURE_LAYOUT::D3D12_TEXTURE_LAYOUT_UNKNOWN;
	texResourceDesc.DepthOrArraySize = 1;
	texResourceDesc.SampleDesc.Count = 1;

	//プロパティ
	D3D12_HEAP_PROPERTIES hprop = {};
	hprop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY::D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	hprop.MemoryPoolPreference = D3D12_MEMORY_POOL::D3D12_MEMORY_POOL_L0;
	hprop.Type = D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_CUSTOM;
	hprop.CreationNodeMask = 1;
	hprop.VisibleNodeMask = 1;

	//白テクスチャデータ
	std::vector<UCHAR> data(4 * 4 * 4);
	//真っ白に染めちゃう
	std::fill(data.begin(), data.end(), 0xff);

	ID3D12Resource* whiteBuffer = nullptr;

	auto result = dev->CreateCommittedResource(
		&hprop,
		D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
		&texResourceDesc,
		D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&whiteBuffer));

	result = whiteBuffer->WriteToSubresource(0, nullptr, data.data(), 4 * 4, 4 * 4 * 4);

	//黒テクスチャ
	std::fill(data.begin(), data.end(), 0x00);

	ID3D12Resource* blackBuffer = nullptr;

	result = dev->CreateCommittedResource(
		&hprop,
		D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
		&texResourceDesc,
		D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&blackBuffer));

	result = blackBuffer->WriteToSubresource(0, nullptr, data.data(), 4 * 4, 4 * 4 * 4);

	std::fill(textureBuffer.begin(), textureBuffer.end(), whiteBuffer);
	std::fill(sphBuffer.begin(), sphBuffer.end(), whiteBuffer);
	std::fill(spaBuffer.begin(), spaBuffer.end(), blackBuffer);

	//StringをWStringに変換
	std::wstring wstr = ConvertStringToWString(path);

	auto folderIndex = wstr.rfind('/');
	auto folderName = wstr.substr(0, folderIndex + 1);

	wstr = wstr.substr(folderIndex + 1);

	auto tmp = wstr;

	//分離
	do {
		wstr = tmp;

		auto i = wstr.rfind('*') != std::string::npos ? wstr.rfind('*') + 1 : 0;

		auto path = wstr.substr(i);

		tmp = wstr.substr(0, i - 1);

		//拡張子を判定
		auto index = path.rfind('.');
		auto filePath = path.substr(index + 1);

		//画像読み込み
		result = loadFuncTbl[filePath](folderName + path, &metaData, image);

		//テクスチャリソースの設定
		texResourceDesc.Width = metaData.width;
		texResourceDesc.Height = metaData.height;

		//バッファ生成
		ID3D12Resource* buffer = nullptr;
		result = dev->CreateCommittedResource(
			&hprop,
			D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
			&texResourceDesc,
			D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&buffer));

		result = buffer->WriteToSubresource(0, nullptr, image.GetPixels(), metaData.width * 4, metaData.height * 4);

		if (filePath == L"sph") {
			sphBuffer[materialIndex] = buffer;
		}
		else if (filePath == L"spa") {
			spaBuffer[materialIndex] = buffer;
		}
		else {
			textureBuffer[materialIndex] = buffer;
		}

	} while (wstr.rfind('*') != std::string::npos);

	return 0;
}

DirectX::TexMetadata ImageLoader::GetMetaData()
{
	return metaData;
}

std::vector<ID3D12Resource*> ImageLoader::GetTextureBuffer()
{
	return textureBuffer;
}

std::vector<ID3D12Resource*> ImageLoader::GetSphBuffer()
{
	return sphBuffer;
}

std::vector<ID3D12Resource*> ImageLoader::GetSpaBuffer()
{
	return spaBuffer;
}

std::map<int, ID3D12Resource*> ImageLoader::GetBufferMap()
{
	return bufferMap;
}

ImageLoader::~ImageLoader() {
}

std::wstring ImageLoader::ConvertStringToWString(std::string str) {
	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> cv;
	return cv.from_bytes(str);
}
