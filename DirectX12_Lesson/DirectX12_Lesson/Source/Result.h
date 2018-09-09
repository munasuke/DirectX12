#pragma once

#include "d3dx12.h"

#define Release(X)	{if((X) != nullptr) (X)->Release();}

class Result
{
public:
	Result();
	~Result();
protected:
	HRESULT result;
};
