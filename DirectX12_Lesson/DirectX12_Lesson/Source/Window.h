#pragma once
#include <Windows.h>
#include <d3d12.h>

constexpr int WIN_WIDTH		= 1280;
constexpr int WIN_HEIGHT	= 720;

class Window {
public:
	Window();

	void InitWindow();	//初期化
	void Show();		//表示

	HWND GetHandleWindow();//ウィンドウハンドルを返す
	D3D12_RECT GetScissorRect();

	~Window();
private:
	//ウィンドウ
	WNDCLASSEX w = {};
	//ウィンドウサイズの設定
	RECT wrc = {0, 0, WIN_WIDTH, WIN_HEIGHT};
	//ウィンドウハンドル
	HWND hwnd = {};
	//シザー矩形
	D3D12_RECT scissorRct = { 0, 0, WIN_WIDTH, WIN_HEIGHT };
};

