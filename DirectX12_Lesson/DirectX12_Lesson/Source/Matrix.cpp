#include <DirectXMath.h>
#include "Matrix.h"
#include "Window.h"

using namespace DirectX;

Matrix::Matrix() : 
	vec({
		{ Eye,		{ 0.0f, 0.0f, -10.0f } },
		{ Target,	{ 0.0f, 0.0f,	0.0f } },
		{ Upper,	{ 0.0f, 1.0f,	0.0f } }
		})
{
	mt.world		= XMMatrixIdentity();
	mt.camera		= XMMatrixLookAtLH(vec[Eye], vec[Target], vec[Upper]);
	mt.projection	= XMMatrixPerspectiveFovLH(0.0f, static_cast<float>(WIN_HEIGHT / WIN_WIDTH), 0.01f, 1000.0f);
}

Matrix::~Matrix() {
}
