#pragma once
#include <Windows.h>

constexpr int WIN_WIDTH		= 1280;
constexpr int WIN_HEIGHT	= 720;

class Window {
public:
	Window();

	void InitWindow();	//初期化
	void Show();		//表示

	HWND GetHandleWindow();//ウィンドウハンドルを返す

	~Window();
private:
	//ウィンドウ
	WNDCLASSEX w = {};
	//ウィンドウサイズの設定
	RECT wrc = {0, 0, WIN_WIDTH, WIN_HEIGHT};
	//ウィンドウハンドル
	HWND hwnd = {};
};

