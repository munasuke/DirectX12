#include "Result.h"
#include "Index.h"



Index::Index() : indexBuffer(nullptr){
}

void Index::Initialize(ID3D12Device* _dev, std::vector<USHORT> _index) {
	result = _dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(_index.size() * sizeof(USHORT)),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuffer)
	);

	UCHAR* pData;
	result = indexBuffer->Map(0, nullptr, (void**)&pData);
	memcpy(pData, &_index[0], _index.size() * sizeof(USHORT));
	indexBuffer->Unmap(0, nullptr);

	ibView.BufferLocation = indexBuffer->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = _index.size() * sizeof(USHORT);
}

D3D12_INDEX_BUFFER_VIEW Index::GetIndexBufferView() {
	return ibView;
}


Index::~Index() {
	ReleaseP(indexBuffer);
}
