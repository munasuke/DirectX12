#pragma once
#include <vector>

class d3d12;

//�V�F�[�_�ǂݍ��݁A�R���p�C��
class ShaderLoader : public Result {
public:
	ShaderLoader();
	int Load(ID3DBlob* _error);//�ǂݍ���
	CD3DX12_SHADER_BYTECODE GetVS();
	CD3DX12_SHADER_BYTECODE GetPS();
	CD3DX12_SHADER_BYTECODE GetPeraVS(unsigned int index);
	CD3DX12_SHADER_BYTECODE GetPeraPS(unsigned int index);
	~ShaderLoader();
private:
	ID3DBlob* vertexShader;	//���_�V�F�[�_
	ID3DBlob* pixelShader;	//�s�N�Z���V�F�[�_

	std::vector<ID3DBlob*> peraVShader;	//���_�V�F�[�_
	std::vector<ID3DBlob*> peraPShader;	//�s�N�Z���V�F�[�_
};

