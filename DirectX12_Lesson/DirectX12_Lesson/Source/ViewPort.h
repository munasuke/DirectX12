#pragma once

//ビューポート
class ViewPort {
public:
	ViewPort();
	//初期化
	void Initialize();
	D3D12_VIEWPORT* GetViewPort();
	~ViewPort();
private:
	D3D12_VIEWPORT viewPort = {};
};

