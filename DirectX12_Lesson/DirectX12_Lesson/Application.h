#pragma once
#include <memory>

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

	std::shared_ptr<Window> window;
};

