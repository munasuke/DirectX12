#include "PrimitiveObject.h"



PrimitiveObject::PrimitiveObject() {
}


PrimitiveObject::~PrimitiveObject() {
}

ID3D12Resource * PrimitiveObject::CreateVBV(ID3D12Device* dev) {
	return vBuffer;
}
