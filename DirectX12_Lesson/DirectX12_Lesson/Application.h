#pragma once

class Application {
public:
	static Application& Instance(){
		static Application instatnce;
		return instatnce;
	}

	void Initialize();	//初期化
	void Run();			//メインループ
	void Terminate();	//終了

	~Application();
private:
	Application();							//生成禁止
	Application(const Application&);		//コピー禁止
	void operator=(const Application&) {};	//代入禁止
};

