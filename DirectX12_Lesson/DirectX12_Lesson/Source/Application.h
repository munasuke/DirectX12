#pragma once
#include <memory>

class Window;
class Device;
class Command;

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

	std::shared_ptr<Window> window;//�E�B���h�E
	std::shared_ptr<Device> device;
	std::shared_ptr<Command> command;
};

