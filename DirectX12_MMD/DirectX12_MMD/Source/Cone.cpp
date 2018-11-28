#include "Cone.h"



Cone::Cone(ID3D12Device* dev, const unsigned int div, const float r) {
	vBuffer = nullptr;
}

void Cone::Draw(ID3D12GraphicsCommandList * list) {
}

ID3D12Resource * Cone::GetVBuffer() {
	return nullptr;
}


Cone::~Cone() {
}
