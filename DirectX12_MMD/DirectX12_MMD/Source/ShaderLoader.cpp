#include <d3d12.h>
#include <D3Dcompiler.h>
#include <tchar.h>
#include "Result.h"
#include "ShaderLoader.h"

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib, "d3dcompiler.lib")

ShaderLoader::ShaderLoader() :vertexShader(nullptr), pixelShader(nullptr){
}

int ShaderLoader::Load(ID3DBlob* _error) {
	//頂点シェーダ
	result = D3DCompileFromFile(
		_T("Shader.hlsl"),
		nullptr,
		nullptr,
		"BasicVS",
		"vs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&vertexShader,
		&_error);

	//ピクセルシェーダ
	result = D3DCompileFromFile(
		_T("Shader.hlsl"),
		nullptr,
		nullptr,
		"BasicPS",
		"ps_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&pixelShader,
		&_error);

	//ペラポリゴン用
	const unsigned int max = 2;
	peraVShader.resize(max);
	peraPShader.resize(max);

	//1パス目
	//頂点シェーダ
	result = D3DCompileFromFile(
		_T("PeraShader.hlsl"),
		nullptr,
		nullptr,
		"BasicVS",
		"vs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&peraVShader[0],
		&_error);

	//ピクセルシェーダ
	result = D3DCompileFromFile(
		_T("PeraShader.hlsl"),
		nullptr,
		nullptr,
		"BasicPS",
		"ps_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&peraPShader[0],
		&_error);

	//2パス目
	//頂点シェーダ
	result = D3DCompileFromFile(
		_T("PeraShader2.hlsl"),
		nullptr,
		nullptr,
		"BasicVS",
		"vs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&peraVShader[1],
		&_error);

	//ピクセルシェーダ
	result = D3DCompileFromFile(
		_T("PeraShader2.hlsl"),
		nullptr,
		nullptr,
		"BasicPS",
		"ps_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&peraPShader[1],
		&_error);

	return 0;
}

CD3DX12_SHADER_BYTECODE ShaderLoader::GetVS() {
	return CD3DX12_SHADER_BYTECODE(vertexShader);
}

CD3DX12_SHADER_BYTECODE ShaderLoader::GetPS() {
	return CD3DX12_SHADER_BYTECODE(pixelShader);
}

CD3DX12_SHADER_BYTECODE ShaderLoader::GetPeraVS(unsigned int index) {
	return CD3DX12_SHADER_BYTECODE(peraVShader[index]);
}

CD3DX12_SHADER_BYTECODE ShaderLoader::GetPeraPS(unsigned int index) {
	return CD3DX12_SHADER_BYTECODE(peraPShader[index]);
}


ShaderLoader::~ShaderLoader() {
	ReleaseP(pixelShader);
	ReleaseP(vertexShader);
}
