#include "Parameter.h"


Parameter::Parameter() {
	SecureZeroMemory(&parameter, sizeof(parameter));
}

void Parameter::InitParameter(D3D12_SHADER_VISIBILITY _shaderVisibility, D3D12_DESCRIPTOR_RANGE _dRange) {
	parameter[0].ParameterType							= D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	parameter[0].ShaderVisibility						= _shaderVisibility;
	parameter[0].DescriptorTable.NumDescriptorRanges	= 2;
	parameter[0].DescriptorTable.pDescriptorRanges		= &_dRange;
}

D3D12_ROOT_PARAMETER* Parameter::GetParameter() {
	return parameter;
}

D3D12_ROOT_PARAMETER Parameter::GetParameter(UINT num) {
	return parameter[num];
}

UINT Parameter::GetParamatorSize() {
	return _countof(parameter);
}


Parameter::~Parameter() {
}
