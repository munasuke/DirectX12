#include "Window.h"
#include <tchar.h>


LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	if (msg == WM_DESTROY) {//ウィンドウが破棄されたら呼ばれる
		PostQuitMessage(0);	//OSに対してアプリの終了を伝える
		return 0;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);//規定の処理を行う
}

Window::Window() {
	w.cbSize		= sizeof(WNDCLASSEX);
	w.lpfnWndProc	= (WNDPROC)WindowProcedure;	//コールバック関数の指定
	w.lpszClassName	= _T("DirectX12");			//アプリケーションクラス名
	w.hInstance		= GetModuleHandle(0);		//ハンドルの取得
	RegisterClassEx(&w);						//OSに伝える

	//ウィンドウサイズの補正
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);
	//ウィンドウ生成
	hwnd = CreateWindow(
		w.lpszClassName,		//クラス名の指定
		_T("DirectX12"),		//タイトルバーの文字
		WS_OVERLAPPEDWINDOW,	//タイトルバーと境界線があるウィンドウ
		CW_USEDEFAULT,			//表示X座標をOSに任せる
		CW_USEDEFAULT,			//表示Y座標をOSに任せる
		wrc.right - wrc.left,	//ウィンドウ幅
		wrc.bottom - wrc.top,	//ウィンドウ高
		nullptr,				//親ウィンドウハンドル
		nullptr,				//メニューハンドル
		w.hInstance,			//呼び出しアプリケーションハンドル
		nullptr					//追加パラメータ
	);
	
	if (hwnd == nullptr){
		
	}
}

Window::~Window() {
}
