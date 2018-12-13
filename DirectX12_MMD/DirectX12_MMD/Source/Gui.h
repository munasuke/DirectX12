#pragma once

#include "d3dx12.h"

//imgui‚ðˆµ‚¤‚½‚ß‚ÌƒNƒ‰ƒX
class Gui {
public:
	Gui();

	void Initialize(ID3D12Device* dev, HWND hwnd);
	void BeforeDrawing();
	void Draw(ID3D12GraphicsCommandList* list);

	~Gui();
private:
	ID3D12DescriptorHeap* heap;
	ID3D12RootSignature* rs;

	bool flag;
};

