#pragma once

#include "d3dx12.h"

//imguiを扱うためのクラス
class Gui {
public:
	Gui();

	void Initialize(ID3D12Device* dev, HWND hwnd);
	void BeforeDrawing();
	void GuiParam(float angle, float rotSpeed);
	void Draw(ID3D12GraphicsCommandList* list);

	bool GetFlag();
	float GetRotationAngle();
	float GetRotationSpeed();

	~Gui();
private:
	ID3D12DescriptorHeap* heap;
	ID3D12RootSignature* rs;

	bool rotationFlag;
	float rotationAngle;
	float rotationSpeed;
};

