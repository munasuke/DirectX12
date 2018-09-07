#include "DescriptorRange.h"



DescriptorRange::DescriptorRange() {
	SecureZeroMemory(&descriptorRange, sizeof(descriptorRange));
}

void DescriptorRange::InitDescriptorRange() {
	descriptorRange[0].RangeType							= D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange[0].NumDescriptors						= 1;
	descriptorRange[0].BaseShaderRegister					= 0;
	descriptorRange[0].RegisterSpace						= 0;
	descriptorRange[0].OffsetInDescriptorsFromTableStart	= D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
}

D3D12_DESCRIPTOR_RANGE* DescriptorRange::GetDescriptorRange() {
	return descriptorRange;
}

D3D12_DESCRIPTOR_RANGE DescriptorRange::GetDescriptorRange(UINT num) {
	return descriptorRange[num];
}


DescriptorRange::~DescriptorRange() {
}
