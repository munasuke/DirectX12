#pragma once
#include <d3d12.h>

class DescriptorRange {
public:
	DescriptorRange();
	void InitDescriptorRange();
	D3D12_DESCRIPTOR_RANGE* GetDescriptorRange();
	D3D12_DESCRIPTOR_RANGE GetDescriptorRange(UINT num);
	~DescriptorRange();
private:
	D3D12_DESCRIPTOR_RANGE descriptorRange[1];
};

