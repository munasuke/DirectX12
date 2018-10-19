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
class Vertex;
class TextureResource;
class ShaderResourceView;
class BmpLoader;
class ImageLoader;
class ShaderLoader;
class PipelineState;
class ViewPort;
class ConstantBuffer;
class PMDLoader;
class Index;
class DepthStencilBuffer;
class Model;
class Camera;

class Application {
public:
	static Application& Instance() {
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

	std::shared_ptr<Window>				window;			//�E�B���h�E
	std::shared_ptr<Device>				device;			//�f�o�C�X
	std::shared_ptr<Command>			command;		//�R�}���h
	std::shared_ptr<SwapChain>			swapChain;		//�X���b�v�`�F�C��
	std::shared_ptr<Descriptor>			descriptor;		//�f�X�N���v�^
	std::shared_ptr<RenderTarget>		renderTarget;	//�����_�[�^�[�Q�b�g
	std::shared_ptr<Sampler>			sampler;		//�T���v��
	std::shared_ptr<RootSignature>		root;			//���[�g�V�O�l�`��
	std::shared_ptr<Fence>				fence;			//�t�F���X
	std::shared_ptr<Vertex>				vertex;			//���_�o�b�t�@
	//std::shared_ptr<TextureResource>	tex;			//�e�N�X�`�����\�[�X
	//std::shared_ptr<ShaderResourceView> srv;			//�V�F�[�_���\�[�X�r���[
	std::shared_ptr<BmpLoader>			bmp;			//BMP
	std::shared_ptr<ImageLoader>		imageL;			//�摜���[�h
	std::shared_ptr<ShaderLoader>		shader;			//�V�F�[�_
	std::shared_ptr<PipelineState>		pipline;		//�p�C�v���C���X�e�[�g�I�u�W�F�N�g
	std::shared_ptr<ViewPort>			viewPort;		//�r���[�|�[�g
	//std::shared_ptr<ConstantBuffer>		cb;				//�R���X�^���g�o�b�t�@
	std::shared_ptr<PMDLoader>			pmd;			//PMD
	std::shared_ptr<Index>				index;			//�C���f�b�N�X
	std::shared_ptr<DepthStencilBuffer>	depth;			//�[�x�o�b�t�@
	std::shared_ptr<Model>				model;			//���f��
	std::shared_ptr<Camera>				camera;			//�J����
};

