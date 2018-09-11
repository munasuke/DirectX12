#include "Result.h"
#include "ConstantBuffer.h"
#include "Matrix.h"

ConstantBuffer::ConstantBuffer() : constantBuffer(nullptr), cbvDescHeap(nullptr){
	matrix = std::make_shared<Matrix>();
}

void ConstantBuffer::Initialize(ID3D12Device * _dev) {
	//_dev->CreateCommittedResource()

	cbvHeapDesc.Type;
	cbvHeapDesc.NumDescriptors;
	cbvHeapDesc.Flags;
	cbvHeapDesc.NodeMask;
}


ConstantBuffer::~ConstantBuffer() {
}
