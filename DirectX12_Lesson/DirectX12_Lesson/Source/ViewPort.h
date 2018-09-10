#pragma once

//ビューポート
class ViewPort {
public:
	ViewPort();
	void Initialize();
	~ViewPort();
private:
	D3D12_VIEWPORT viewPort = {};
};

