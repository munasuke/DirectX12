#pragma once

//�r���[�|�[�g
class ViewPort {
public:
	ViewPort();
	void Initialize();
	~ViewPort();
private:
	D3D12_VIEWPORT viewPort = {};
};

