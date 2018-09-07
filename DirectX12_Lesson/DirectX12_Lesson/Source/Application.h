#pragma once
#include <memory>

class Window;
class Device;
class Command;
class SwapChain;
class Descriptor;
class RenderTarget;
class Sampler;
class DescriptorRange;
class Parameter;
class RootSignature;
class Fence;

class Application {
public:
	static Application& Instance(){
		static Application instatnce;
		return instatnce;
	}

	void Initialize();	//������
	void Run();			//���C�����[�v
	void Terminate();	//�I��

	~Application();
private:
	Application();							//�����֎~
	Application(const Application&);		//�R�s�[�֎~
	void operator=(const Application&) {};	//����֎~

	std::shared_ptr<Window> window;				//�E�B���h�E
	std::shared_ptr<Device> device;				//�f�o�C�X
	std::shared_ptr<Command> command;			//�R�}���h
	std::shared_ptr<SwapChain> swapChain;		//�X���b�v�`�F�C��
	std::shared_ptr<Descriptor> descriptor;		//�f�X�N���v�^
	std::shared_ptr<RenderTarget> renderTarget;	//�����_�[�^�[�Q�b�g
	std::shared_ptr<Sampler> sampler;			//�T���v��
	std::shared_ptr<DescriptorRange> dRange;	//�f�X�N���v�^�����W
	std::shared_ptr<Parameter> parameter;		//�p�����[�^
	std::shared_ptr<RootSignature> root;		//���[�g�V�O�l�`��
	std::shared_ptr<Fence> fence;				//�t�F���X
};

