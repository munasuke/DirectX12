#include "ImageLoader.h"
#include <locale>
#include <codecvt>

using namespace DirectX;

ImageLoader::ImageLoader(ID3D12Device* dev) : dev(dev){
	//�g���q�Ń��[�h�֐���ύX
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
	//�}�e���A�����̃o�b�t�@���m��
	textureBuffer.resize(materialSize);
	sphBuffer.resize(materialSize);
	spaBuffer.resize(materialSize);

	//�e�N�X�`���f�[�^
	std::vector<UCHAR> data(4 * 4 * 4);

	//�^�����ɐ��߂��Ⴄ
	std::fill(data.begin(), data.end(), 0xff);
	//���e�N�X�`���o�b�t�@����
	ID3D12Resource* whiteBuffer = CreateBuffer(4, 4);
	auto result = whiteBuffer->WriteToSubresource(0, nullptr, data.data(), 4 * 4, 4 * 4 * 4);

	//�^�����ɐ��߂��Ⴄ
	std::fill(data.begin(), data.end(), 0x00);
	//���e�N�X�`���o�b�t�@����
	ID3D12Resource* blackBuffer = CreateBuffer(4, 4);
	result = blackBuffer->WriteToSubresource(0, nullptr, data.data(), 4 * 4, 4 * 4 * 4);

	//�ʏ�E��Z�e�N�X�`���ɂ͔��A���Z�e�N�X�`���ɂ͍������Ă���
	std::fill(textureBuffer.begin(), textureBuffer.end(), whiteBuffer);
	std::fill(sphBuffer.begin(), sphBuffer.end(), whiteBuffer);
	std::fill(spaBuffer.begin(), spaBuffer.end(), blackBuffer);

	return 0;
}

int ImageLoader::Load(const std::string path, int materialsize, int materialIndex) {
	//String��WString�ɕϊ�
	std::wstring wstr = ConvertStringToWString(path);

	//�t�H���_�p�X���擾
	auto folderIndex = wstr.rfind('/');
	auto folderName = wstr.substr(0, folderIndex + 1);

	//�t�H���_�p�X�ȍ~���擾
	wstr = wstr.substr(folderIndex + 1);
	auto tmp = wstr;

	do {
		//�p�X�̏�������
		wstr = tmp;

		//�u*�v�����邩����
		auto i = wstr.rfind('*') != std::string::npos ? wstr.rfind('*') + 1 : 0;

		//�u*�v�ȍ~�̃p�X���擾
		auto path = wstr.substr(i);

		//�c�����p�X���擾
		tmp = wstr.substr(0, i - 1);

		//�g���q�𔻒�
		auto index = path.rfind('.');
		auto filePath = path.substr(index + 1);

		//�摜�ǂݍ���
		auto result = loadFuncTbl[filePath](folderName + path, &metaData, image);

		//�o�b�t�@����
		ID3D12Resource* buffer = CreateBuffer(metaData.width, metaData.height, metaData.format);
		result = buffer->WriteToSubresource(0, nullptr, image.GetPixels(), metaData.width * 4, metaData.height * 4);

		if (filePath == L"sph") {
			//��Z�e�N�X�`��
			sphBuffer[materialIndex] = buffer;
		}
		else if (filePath == L"spa") {
			//���Z�e�N�X�`��
			spaBuffer[materialIndex] = buffer;
		}
		else {
			//�ʏ�e�N�X�`��
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

ImageLoader::~ImageLoader() {
}

std::wstring ImageLoader::ConvertStringToWString(std::string str) {
	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> cv;
	return cv.from_bytes(str);
}

ID3D12Resource * ImageLoader::CreateBuffer(int width, int height, DXGI_FORMAT format) {
	//�e�N�X�`�����\�[�X�̐ݒ�
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

	//�v���p�e�B
	D3D12_HEAP_PROPERTIES hprop = {};
	hprop.CPUPageProperty		= D3D12_CPU_PAGE_PROPERTY::D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	hprop.MemoryPoolPreference	= D3D12_MEMORY_POOL::D3D12_MEMORY_POOL_L0;
	hprop.Type					= D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_CUSTOM;
	hprop.CreationNodeMask		= 1;
	hprop.VisibleNodeMask		= 1;

	//�o�b�t�@����
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
