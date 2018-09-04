#pragma once
#include <Windows.h>

constexpr int WIN_WIDTH		= 640;
constexpr int WIN_HEIGHT = 480;

class Window {
public:
	Window();

	void InitWindow();	//初期化
	void Show();		//表示

	~Window();
private:
	//ウィンドウ
	WNDCLASSEX w = {};
	//ウィンドウサイズの設定
	RECT wrc = {0, 0, WIN_WIDTH, WIN_HEIGHT};
	//ウィンドウハンドル
	HWND hwnd = {};
};

