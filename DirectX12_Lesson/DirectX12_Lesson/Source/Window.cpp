#include "Window.h"
#include <tchar.h>
#include <d3d12.h>


LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	if (msg == WM_DESTROY) {//�E�B���h�E���j�����ꂽ��Ă΂��
		PostQuitMessage(0);	//OS�ɑ΂��ăA�v���̏I����`����
		return 0;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);//�K��̏������s��
}

Window::Window() {
}

void Window::InitWindow() {
	w.cbSize		= sizeof(WNDCLASSEX);
	w.lpfnWndProc	= (WNDPROC)WindowProcedure;	//�R�[���o�b�N�֐��̎w��
	w.lpszClassName	= _T("DirectX12");			//�A�v���P�[�V�����N���X��
	w.hInstance		= GetModuleHandle(0);		//�n���h���̎擾
	RegisterClassEx(&w);						//OS�ɓ`����

	//�E�B���h�E�T�C�Y�̕␳
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);
	//�E�B���h�E����
	hwnd = CreateWindow(
		w.lpszClassName,		//�N���X���̎w��
		_T("DirectX12"),		//�^�C�g���o�[�̕���
		WS_OVERLAPPEDWINDOW,	//�^�C�g���o�[�Ƌ��E��������E�B���h�E
		CW_USEDEFAULT,			//�\��X���W��OS�ɔC����
		CW_USEDEFAULT,			//�\��Y���W��OS�ɔC����
		wrc.right - wrc.left,	//�E�B���h�E��
		wrc.bottom - wrc.top,	//�E�B���h�E��
		nullptr,				//�e�E�B���h�E�n���h��
		nullptr,				//���j���[�n���h��
		w.hInstance,			//�Ăяo���A�v���P�[�V�����n���h��
		nullptr					//�ǉ��p�����[�^
	);

	//���s���L���b�`
	if (hwnd == nullptr){
		LPVOID messageBuffer = nullptr;
		//�G���[���N������R�}���h�v�����v�g�ɏo��
		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			nullptr,
			GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPWSTR)messageBuffer,
			0,
			nullptr
		);
	}
}

void Window::Show() {
	//�E�B���h�E�\��
	ShowWindow(hwnd, SW_SHOW);
}

HWND Window::GetHandleWindow() {
	return hwnd;
}

D3D12_RECT* Window::GetScissorRect()
{
	return &scissorRct;
}

Window::~Window() {
	//�o�^����
	UnregisterClass(w.lpszClassName, w.hInstance);
}

