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
	* 初期化関数
	* @brief	ウィンドウを生成するために必要な設定をします。
	* @detail	構造体の設定、サイズの補正をして、Create関数でウィンドウを生成します。
	*/
	void InitWindow();

	/*!
	* @fn
	* ウィンドウを表示する関数
	* @brief	ShowWindowでウィンドウを表示します。
	*/
	void Show();

	/*!
	* @fn
	* ウィンドウハンドルを返す関数
	* @brief	ウィンドウハンドルを返します。
	* @return	ウィンドウハンドルを返します。
	*/
	HWND GetHandleWindow();

	/*!
	* @fn
	* シザーレクトを返す関数
	* @brief	シザーレクトを返します。
	* @return	シザーレクトを返します。
	*/
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

