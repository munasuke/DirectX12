#pragma once
#include <Windows.h>
#include <d3d12.h>

constexpr int WIN_WIDTH		= 1280;
constexpr int WIN_HEIGHT	= 720;

class Window {
public:
	Window();

	/*!
	* @fn
	* �������֐�
	* @brief	�E�B���h�E�𐶐����邽�߂ɕK�v�Ȑݒ�����܂��B
	* @detail	�\���̂̐ݒ�A�T�C�Y�̕␳�����āACreate�֐��ŃE�B���h�E�𐶐����܂��B
	*/
	void InitWindow();

	/*!
	* @fn
	* �E�B���h�E��\������֐�
	* @brief	ShowWindow�ŃE�B���h�E��\�����܂��B
	*/
	void Show();

	/*!
	* @fn
	* �E�B���h�E�n���h����Ԃ��֐�
	* @brief	�E�B���h�E�n���h����Ԃ��܂��B
	* @return	�E�B���h�E�n���h����Ԃ��܂��B
	*/
	HWND GetHandleWindow();

	/*!
	* @fn
	* �V�U�[���N�g��Ԃ��֐�
	* @brief	�V�U�[���N�g��Ԃ��܂��B
	* @return	�V�U�[���N�g��Ԃ��܂��B
	*/
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

