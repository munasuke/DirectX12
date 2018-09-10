#include "ViewPort.h"
#include "Window.h"
#include <d3d12.h>


ViewPort::ViewPort() {
}

void ViewPort::Initialize() {
	//ビューポートの作成
	viewPort.Width		= WIN_WIDTH;
	viewPort.Height		= WIN_HEIGHT;
	viewPort.TopLeftX	= 0.0f;
	viewPort.TopLeftY	= 0.0f;
	viewPort.MaxDepth	= 1.0f;
	viewPort.MinDepth	= 0.0f;
}

D3D12_VIEWPORT* ViewPort::GetViewPort()
{
	return &viewPort;
}


ViewPort::~ViewPort() {
}
