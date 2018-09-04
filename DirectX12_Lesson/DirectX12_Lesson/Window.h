#pragma once
#include <Windows.h>

#define WIN_WIDTH	480
#define WIN_HEIGHT	640

class Window {
public:
	Window();

	void InitWindow();//�E�B���h�E������
	void Show();//�E�B���h�E�\��

	~Window();
private:
	WNDCLASSEX w = {};

	//�E�B���h�E�T�C�Y�̐ݒ�
	RECT wrc = {0, 0, WIN_WIDTH, WIN_HEIGHT};

	HWND hwnd = {};
};

