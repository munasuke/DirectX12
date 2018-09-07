#pragma once
#include <d3d12.h>

//ルートパラメータ
class Parameter {
public:
	Parameter();
	void InitParameter(D3D12_SHADER_VISIBILITY _shaderVisibility, D3D12_DESCRIPTOR_RANGE _dRange);
	D3D12_ROOT_PARAMETER* GetParameter();
	D3D12_ROOT_PARAMETER GetParameter(UINT num);
	UINT GetParamatorSize();

	~Parameter();
private:
	D3D12_ROOT_PARAMETER parameter[1];
};

