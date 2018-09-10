#include "Window.h"
#include <tchar.h>
#include <d3d12.h>


LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	if (msg == WM_DESTROY) {//ウィンドウが破棄されたら呼ばれる
		PostQuitMessage(0);	//OSに対してアプリの終了を伝える
		return 0;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);//規定の処理を行う
}

Window::Window() {
}

void Window::InitWindow() {
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

	//失敗をキャッチ
	if (hwnd == nullptr){
		LPVOID messageBuffer = nullptr;
		//エラーが起きたらコマンドプロンプトに出力
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
	//ウィンドウ表示
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
	//登録解除
	UnregisterClass(w.lpszClassName, w.hInstance);
}

