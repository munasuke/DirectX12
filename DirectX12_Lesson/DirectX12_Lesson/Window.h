#pragma once
#include <Windows.h>

#define WIN_WIDTH	480
#define WIN_HEIGHT	640

class Window {
public:
	Window();
	~Window();
private:
	WNDCLASSEX w = {};

	//ウィンドウサイズの設定
	RECT wrc = {0, 0, WIN_WIDTH, WIN_HEIGHT};

	HWND hwnd = {};
};

