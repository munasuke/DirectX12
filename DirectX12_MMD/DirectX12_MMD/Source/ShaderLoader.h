#pragma once
#include <vector>

class d3d12;

//シェーダ読み込み、コンパイル
class ShaderLoader : public Result {
public:
	ShaderLoader();
	int Load(ID3DBlob* _error);//読み込み
	CD3DX12_SHADER_BYTECODE GetVS();
	CD3DX12_SHADER_BYTECODE GetPS();
	CD3DX12_SHADER_BYTECODE GetPeraVS(unsigned int index);
	CD3DX12_SHADER_BYTECODE GetPeraPS(unsigned int index);
	~ShaderLoader();
private:
	ID3DBlob* vertexShader;	//頂点シェーダ
	ID3DBlob* pixelShader;	//ピクセルシェーダ

	std::vector<ID3DBlob*> peraVShader;	//頂点シェーダ
	std::vector<ID3DBlob*> peraPShader;	//ピクセルシェーダ
};

