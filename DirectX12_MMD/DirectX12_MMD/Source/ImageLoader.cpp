#include "ImageLoader.h"
#include <locale>
#include <codecvt>
#include <Shlwapi.h>

#pragma comment(lib, "shlwapi.lib")

using namespace DirectX;

ImageLoader::ImageLoader(ID3D12Device* dev) : dev(dev), toonFlag(false){
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

int ImageLoader::Initialize(int materialSize) {
	//マテリアル分のバッファを確保
	textureBuffer.resize(materialSize);
	sphBuffer.resize(materialSize);
	spaBuffer.resize(materialSize);
	toonBuffer.resize(materialSize);

	//テクスチャデータ
	std::vector<UCHAR> data(4 * 4 * 4);

	//真っ白に染めちゃう
	std::fill(data.begin(), data.end(), 0xff);
	//白テクスチャバッファ生成
	ID3D12Resource* whiteBuffer = CreateBuffer(4, 4);
	auto result = whiteBuffer->WriteToSubresource(0, nullptr, data.data(), 4 * 4, 4 * 4 * 4);

	//真っ黒に染めちゃう
	std::fill(data.begin(), data.end(), 0x00);
	//黒テクスチャバッファ生成
	ID3D12Resource* blackBuffer = CreateBuffer(4, 4);
	result = blackBuffer->WriteToSubresource(0, nullptr, data.data(), 4 * 4, 4 * 4 * 4);

	//グラデーション
	ID3D12Resource* gradBuffer = CreateBuffer(4, 256);
	struct Color {
		unsigned char r, g, b, a;
		Color() : r(0), g(0), b(0), a(0) {}
		Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a) : r(r), g(g), b(b), a(a) {}
	};
	std::vector<Color> gData(4 * 256);
	//明るさ
	unsigned char brightness = 255;
	//1行ごとに明るさを下げていく
	for (auto it = gData.begin(); it != gData.end(); it += 4) {
		std::fill_n(it, 4, Color(brightness, brightness, brightness, 0xff));
		--brightness;
	}
	result = gradBuffer->WriteToSubresource(0, nullptr, gData.data(), sizeof(Color) * 4, sizeof(Color) * gData.size());

	//通常・乗算テクスチャには白、加算テクスチャには黒、トゥーンにはグラデーションを入れておく
	std::fill(textureBuffer.begin(), textureBuffer.end(), whiteBuffer);
	std::fill(sphBuffer.begin(), sphBuffer.end(), whiteBuffer);
	std::fill(spaBuffer.begin(), spaBuffer.end(), blackBuffer);
	std::fill(toonBuffer.begin(), toonBuffer.end(), gradBuffer);

	return 0;
}

int ImageLoader::Load(const std::string path, int materialsize, int materialIndex) {
	//StringをWStringに変換
	std::wstring wstr = ConvertStringToWString(path);

	//フォルダパスを取得
	auto folderIndex = wstr.rfind('/');
	auto folderName = wstr.substr(0, folderIndex + 1);

	//フォルダパス以降を取得
	wstr = wstr.substr(folderIndex + 1);
	auto tmp = wstr;

	do {
		//パスの書き換え
		wstr = tmp;

		//「*」があるか検索
		auto i = wstr.rfind('*') != std::string::npos ? wstr.rfind('*') + 1 : 0;

		//「*」以降のパスを取得
		auto path = wstr.substr(i);

		//残ったパスを取得
		tmp = wstr.substr(0, i - 1);

		//拡張子を判定
		auto index = path.rfind('.');
		auto filePath = path.substr(index + 1);

		//画像読み込み
		auto result = loadFuncTbl[filePath](folderName + path, &metaData, image);

		//バッファ生成
		ID3D12Resource* buffer = CreateBuffer(metaData.width, metaData.height, metaData.format);
		result = buffer->WriteToSubresource(0, nullptr, image.GetPixels(), metaData.width * 4, metaData.height * 4);

		if (toonFlag) {
			//トゥーンテクスチャ
			toonBuffer[materialIndex] = buffer;
			toonFlag = false;
			return 0;
		}
		if (filePath == L"sph") {
			//乗算テクスチャ
			sphBuffer[materialIndex] = buffer;
		}
		else if (filePath == L"spa") {
			//加算テクスチャ
			spaBuffer[materialIndex] = buffer;
		}
		else {
			//通常テクスチャ
			textureBuffer[materialIndex] = buffer;
		}
	} while (wstr.rfind('*') != std::string::npos);

	return 0;
}

std::vector<ID3D12Resource*> ImageLoader::GetTextureBuffer() {
	return textureBuffer;
}

std::vector<ID3D12Resource*> ImageLoader::GetSphBuffer() {
	return sphBuffer;
}

std::vector<ID3D12Resource*> ImageLoader::GetSpaBuffer() {
	return spaBuffer;
}

std::vector<ID3D12Resource*> ImageLoader::GetToonBuffer() {
	return toonBuffer;
}

std::string ImageLoader::GetToonPathFromIndex(const std::string folder, const char* toonName) {
	std::string fileName = toonName;
	std::string path = "Toon/";
	path += fileName;
	toonFlag = true;
	//トゥーンフォルダを探し、見つからなければモデルフォルダを探す
	if (PathFileExistsA(path.c_str())) {
		return path;
	}
	else {
		return folder + path;
	}
}

ImageLoader::~ImageLoader() {
}

std::wstring ImageLoader::ConvertStringToWString(std::string str) {
	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> cv;
	return cv.from_bytes(str);
}

ID3D12Resource * ImageLoader::CreateBuffer(int width, int height, DXGI_FORMAT format) {
	//テクスチャリソースの設定
	D3D12_RESOURCE_DESC texResourceDesc = {};
	CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	texResourceDesc.Width				= width;
	texResourceDesc.Height				= height;
	texResourceDesc.Dimension			= D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	texResourceDesc.Format				= format;
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
	auto result = dev->CreateCommittedResource(
		&hprop,
		D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
		&texResourceDesc,
		D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&buffer));

	return buffer;
}
