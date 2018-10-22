#pragma once

class d3d12;

//�V�F�[�_�ǂݍ��݁A�R���p�C��
class ShaderLoader : public Result {
public:
	ShaderLoader();
	int Load(ID3DBlob* _error);//�ǂݍ���
	CD3DX12_SHADER_BYTECODE GetVS();
	CD3DX12_SHADER_BYTECODE GetPS();
	~ShaderLoader();
private:
	ID3DBlob* vertexShader;	//���_�V�F�[�_
	ID3DBlob* pixelShader;	//�s�N�Z���V�F�[�_
};
