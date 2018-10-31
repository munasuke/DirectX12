#include "Model.h"
#include "PMDLoader.h"
#include "ImageLoader.h"
#include "VMDMotion.h"
#include <iostream>

using namespace DirectX;

Model::Model(std::shared_ptr<PMDLoader> pmd, std::shared_ptr<ImageLoader> img, std::shared_ptr<VMDMotion> vmd) :
	pmd(pmd),
	img(img),
	vmd(vmd),
	heap(nullptr), 
	whiteBuffer(nullptr), 
	blackBuffer(nullptr),
	boneBuffer(nullptr),
	boneMatrixData(nullptr)
{
}

void Model::Initialize(ID3D12Device * _dev) {
	auto material = pmd.lock()->GetMaterial();
	//�q�[�v�̐ݒ�
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.NumDescriptors = material.size() * 2;
	heapDesc.Type			= D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	heapDesc.Flags			= D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	heapDesc.NodeMask		= 0;

	//�q�[�v����
	auto result = _dev->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&heap));

	//�}�e���A�����̃o�b�t�@�𐶐�
	auto size = ((sizeof(DirectX::XMFLOAT3) + 0xff) &~0xff);
	materialBuffer.resize(material.size());
	for (UINT i = 0; i < materialBuffer.size(); ++i) {
		auto result = _dev->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(size),
			D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&materialBuffer[i])
		);

		//�f�B�t���[�Y����
		mat.diffuse.x = material[i].diffuse.x;
		mat.diffuse.y = material[i].diffuse.y;
		mat.diffuse.z = material[i].diffuse.z;
		mat.diffuse.w = material[i].alpha;

		//�X�y�L��������
		mat.specular.x = material[i].specular.x;
		mat.specular.y = material[i].specular.y;
		mat.specular.z = material[i].specular.z;
		mat.specular.w = material[i].specularPower;

		//�A���r�G���g����
		mat.ambient.x = material[i].ambient.x;
		mat.ambient.y = material[i].ambient.y;
		mat.ambient.z = material[i].ambient.z;

		mat.texFlag = material[i].textureFilePath[0] != '\0' ? true : false;
		texFlag.emplace_back(mat.texFlag);

		result = materialBuffer[i]->Map(0, nullptr, (void**)&data);

		memcpy(data, &mat, sizeof(Materials));

		materialBuffer[i]->Unmap(0, nullptr);
	}

	//�e�N�X�`�����\�[�X�̐ݒ�
	D3D12_RESOURCE_DESC texResourceDesc = {};
	CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	texResourceDesc.Dimension			= D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	texResourceDesc.Format				= DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
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

	//���e�N�X�`���o�b�t�@�̐���
	texResourceDesc.Width	= 4;
	texResourceDesc.Height	= 4 * 4;
	result = _dev->CreateCommittedResource(
		&hprop,
		D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
		&texResourceDesc,
		D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&whiteBuffer));
	result = _dev->CreateCommittedResource(
		&hprop,
		D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
		&texResourceDesc,
		D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&blackBuffer));

	//���e�N�X�`������
	CreateWhiteTexture();

	//���e�N�X�`������
	CreateBlackTexture();

	//SRV�̐ݒ�
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format					= DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension			= D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels		= 1;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	//�q�[�v�̊J�n��\��CPU�f�X�N���v�^�n���h��
	auto handle = heap->GetCPUDescriptorHandleForHeapStart();

	//����CPU�f�X�N���v�^�n���h���܂ł̃T�C�Y
	auto incrementSize = _dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	//debug
	auto tBuffIndex = 0;
	auto buffLocation = ((sizeof(DirectX::XMFLOAT3) + 0xff) &~0xff);
	//�r���[����
	for (UINT i = 0; i < materialBuffer.size(); ++i) {
		//CBV�̐ݒ�
		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
		cbvDesc.SizeInBytes		= buffLocation;
		cbvDesc.BufferLocation	= materialBuffer[i]->GetGPUVirtualAddress();

		//CBV����
		_dev->CreateConstantBufferView(&cbvDesc, handle);

		//�ʏ�e�N�X�`���܂ł��炷
		handle.ptr += incrementSize;

		//SRV����
		//�}�e���A���Ƀe�N�X�`��������Ȃ�e�N�X�`�����A�Ȃ��Ȃ甒�e�N�X�`���𐶐�
		if (texFlag[i]) {
			_dev->CreateShaderResourceView(img.lock()->GetTextureBuffer()[tBuffIndex], &srvDesc, handle);
			++tBuffIndex;
		}
		else {
			_dev->CreateShaderResourceView(whiteBuffer, &srvDesc, handle);
		}

		//�}�e���A���܂ł��炷
		handle.ptr += incrementSize;
	}

	//�{�[���o�b�t�@����
	CreateBoneBuffer(_dev);
}

void Model::Update() {
	static int frameNo = 0;
	MotionUpdate(frameNo / 2);
	++frameNo;
}

void Model::Draw(ID3D12GraphicsCommandList * _list, ID3D12Device * _dev) {
	auto material = pmd.lock()->GetMaterial();
	//���̃}�e���A���܂ł̃I�t�Z�b�g
	UINT offset = 0;

	//�q�[�v�̊J�n��\��GPU�f�X�N���v�^�n���h��
	auto handle = heap->GetGPUDescriptorHandleForHeapStart();

	//�q�[�v�̃Z�b�g
	ID3D12DescriptorHeap* heaps[] = { heap };
	_list->SetDescriptorHeaps(1, heaps);

	//�{�[���̃Z�b�g
	_list->SetGraphicsRootConstantBufferView(2, boneBuffer->GetGPUVirtualAddress());

	for (UINT i = 0; i < material.size(); ++i) {
		//�}�e���A���̃Z�b�g
		_list->SetGraphicsRootDescriptorTable(1, handle);

		//�ʏ�e�N�X�`���܂ł��炷
		handle.ptr += 2 * _dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		//�}�e���A���ʂɕ`��
		_list->DrawIndexedInstanced(material[i].indexCount, 1, offset, 0, 0);

		//���̃}�e���A���܂ŃI�t�Z�b�g����
		offset += material[i].indexCount;
	}
}

void Model::CreateWhiteTexture() {
	//���e�N�X�`���f�[�^
	std::vector<UCHAR> data(4 * 4 * 4);
	//�^�����ɐ��߂��Ⴄ
	std::fill(data.begin(), data.end(), 0xff);

	//��������
	auto result = whiteBuffer->WriteToSubresource(0, nullptr, data.data(), 4 * 4, 4 * 4 * 4);
}

void Model::CreateBlackTexture() {
	//���e�N�X�`���f�[�^
	std::vector<UCHAR> data(4 * 4 * 4);
	//�^�����ɐ��߂��Ⴄ
	std::fill(data.begin(), data.end(), 0x00);

	//��������
	auto result = blackBuffer->WriteToSubresource(0, nullptr, data.data(), 4 * 4, 4 * 4 * 4);
}


void Model::CreateBoneBuffer(ID3D12Device * dev) {
	//�{�[���o�b�t�@����
	CD3DX12_HEAP_PROPERTIES bHeapProp = {};
	bHeapProp.Type					= D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD;
	bHeapProp.MemoryPoolPreference	= D3D12_MEMORY_POOL::D3D12_MEMORY_POOL_UNKNOWN;
	bHeapProp.CPUPageProperty		= D3D12_CPU_PAGE_PROPERTY::D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	bHeapProp.CreationNodeMask		= 1;
	bHeapProp.VisibleNodeMask		= 1;

	D3D12_RESOURCE_DESC bResDesc = {};
	bResDesc.Flags				= D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_NONE;
	bResDesc.Format				= DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
	bResDesc.Layout				= D3D12_TEXTURE_LAYOUT::D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	bResDesc.Dimension			= D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_BUFFER;
	bResDesc.Height				= 1;
	bResDesc.Width				= ((sizeof(XMFLOAT3) + 0xff) &~0xff) * pmd.lock()->GetBoneData().size();
	bResDesc.DepthOrArraySize	= 1;
	bResDesc.Alignment			= 0;
	bResDesc.MipLevels			= 1;
	bResDesc.SampleDesc.Count	= 1;

	auto result = dev->CreateCommittedResource(
		&bHeapProp,
		D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
		&bResDesc,
		D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&boneBuffer)
	);

	result = boneBuffer->Map(0, nullptr, (void**)&boneMatrixData);
}

void Model::RecursiveMatrixMultiply(BoneNode * node, DirectX::XMMATRIX & mat) {
	auto& boneMatrix = pmd.lock()->boneMatrices[node->boneIndex];
	boneMatrix *= mat;
	for (auto& cnode : node->children) {
		RecursiveMatrixMultiply(cnode, boneMatrix);
	}
}

void Model::RotationBone(const std::string str, const DirectX::XMFLOAT4 & angle, const DirectX::XMFLOAT4 & q2, float t) {
	auto node = pmd.lock()->boneMap[str];
	auto vec = XMLoadFloat3(&node.startPos);
	auto quaternion = XMLoadFloat4(&angle);
	auto quaternion2 = XMLoadFloat4(&q2);
	pmd.lock()->boneMatrices[node.boneIndex] = XMMatrixTranslationFromVector(
		XMVectorScale(vec, -1.0f))
		* XMMatrixRotationQuaternion(XMQuaternionSlerp(quaternion, quaternion2, t))
		* XMMatrixTranslationFromVector(vec);
}

void Model::MotionUpdate(int framNo) {
	auto& boneMatrix = pmd.lock()->boneMatrices;

	//�{�[���s��̏�����
	std::fill(boneMatrix.begin(), boneMatrix.end(), XMMatrixIdentity());

	//�f�[�^����|�[�W���O�K�p
	for (auto& anim : vmd.lock()->GetAnimationData()) {
		auto keyFlame = anim.second;
		auto frameIt = std::find_if(keyFlame.rbegin(), keyFlame.rend(),
			[framNo](const MotionData& m) { return m.frameNo <= framNo; });
		if (frameIt == keyFlame.rend()) {
			continue;
		}
		auto nextIt = frameIt.base();
		if (nextIt == keyFlame.end()) {
			RotationBone(anim.first.c_str(), frameIt->quaternion);
		}
		else {
			float a = frameIt->frameNo;
			float b = nextIt->frameNo;
			float t = static_cast<float>(framNo - a) / (b - a);
			t = GetVezierYValueFromXWithNewton(t, frameIt->bz[0], frameIt->bz[1]);
			RotationBone(anim.first.c_str(), frameIt->quaternion, nextIt->quaternion, t);
		}
	}
	//�c���[���g���o�[�X
	XMMATRIX root = XMMatrixIdentity();
	RecursiveMatrixMultiply(&pmd.lock()->boneMap["�Z���^�["], root);

	memcpy(boneMatrixData, boneMatrix.data(), ((sizeof(XMMATRIX) + 0xff) &~ 0xff) * pmd.lock()->GetBoneData().size());
}

float Model::GetVezierYValueFromXWithNewton(float x, DirectX::XMFLOAT2 a, DirectX::XMFLOAT2 b, unsigned int n) {
	//�����������ꍇ�͌v�Z�����ɔ�����
	if (a.x == a.y && b.x == b.y) {
		return x;
	}

	//�ŏI�I�ɋ��߂����}��ϐ�
	float t = x;				//�����l��x�Ɠ���

								//�W��
	float k[] = {
		1 + 3 * a.x - 3 * b.x,	//t^3�̌W��
		3 * b.x - 6 * a.x,		//t^2�̌W��
		3 * a.x					//t�̌W��
	};

	//�덷�͈͓̔����ǂ����Ɏg�p����萔
	const float epsilon = 0.0005f;

	//�j���[�g���@
	for (int i = 0; i < n; ++i) {
		//f(t)�����߂�
		float ft = (t * t * t) * k[0] + (t * t) * k[1] + t * k[2] - x;

		//�덷�͈͓̔��Ȃ�Όv�Z�I��
		if (ft <= epsilon && ft >= -epsilon) {
			break;
		}

		//f'(t)�����߂�(f(t)�̔�������)
		float fdt = 3 * (t * t) * k[0] + 2 * t * k[1] + k[2];

		//0���Z��h�~����
		if (fdt == 0) {
			break;
		}

		//���ɋ߂Â���
		t = t - ft / fdt;
	}

	//���]
	float r = 1 - t;

	return 3 * (r * r * t * a.y) + 3 * (r * t * t * b.y) + (t * t * t);
}

Model::~Model() {
}
