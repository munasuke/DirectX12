#pragma once

//�r���[�|�[�g
class ViewPort {
public:
	ViewPort();
	//������
	void Initialize();
	D3D12_VIEWPORT* GetViewPort();
	~ViewPort();
private:
	D3D12_VIEWPORT viewPort = {};
};

