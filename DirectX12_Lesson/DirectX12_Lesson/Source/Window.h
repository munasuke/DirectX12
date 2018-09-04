#pragma once
#include <Windows.h>

constexpr int WIN_WIDTH		= 640;
constexpr int WIN_HEIGHT = 480;

class Window {
public:
	Window();

	void InitWindow();	//������
	void Show();		//�\��

	~Window();
private:
	//�E�B���h�E
	WNDCLASSEX w = {};
	//�E�B���h�E�T�C�Y�̐ݒ�
	RECT wrc = {0, 0, WIN_WIDTH, WIN_HEIGHT};
	//�E�B���h�E�n���h��
	HWND hwnd = {};
};

