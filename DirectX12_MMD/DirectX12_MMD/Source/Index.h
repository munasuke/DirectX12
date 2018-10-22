#pragma once
#include "PMDLoader.h"
#include <vector>

class Index : public Result {
public:
	Index();
	void Initialize(ID3D12Device* _dev, std::vector<USHORT> _index);
	D3D12_INDEX_BUFFER_VIEW GetIndexBufferView();
	~Index();
private:
	ID3D12Resource * indexBuffer;
	D3D12_INDEX_BUFFER_VIEW ibView = {};
};

