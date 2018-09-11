#pragma once
#include <Windows.h>
#include <d3d12.h>

constexpr int WIN_WIDTH		= 1280;
constexpr int WIN_HEIGHT	= 720;

class Window {
public:
	Window();

	void InitWindow();	//������
	void Show();		//�\��

	HWND GetHandleWindow();//�E�B���h�E�n���h����Ԃ�
	D3D12_RECT GetScissorRect();

	~Window();
private:
	//�E�B���h�E
	WNDCLASSEX w = {};
	//�E�B���h�E�T�C�Y�̐ݒ�
	RECT wrc = {0, 0, WIN_WIDTH, WIN_HEIGHT};
	//�E�B���h�E�n���h��
	HWND hwnd = {};
	//�V�U�[��`
	D3D12_RECT scissorRct = { 0, 0, WIN_WIDTH, WIN_HEIGHT };
};

