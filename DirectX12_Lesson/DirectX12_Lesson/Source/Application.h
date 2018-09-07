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

	void Initialize();	//初期化
	void Run();			//メインループ
	void Terminate();	//終了

	~Application();
private:
	Application();							//生成禁止
	Application(const Application&);		//コピー禁止
	void operator=(const Application&) {};	//代入禁止

	std::shared_ptr<Window> window;				//ウィンドウ
	std::shared_ptr<Device> device;				//デバイス
	std::shared_ptr<Command> command;			//コマンド
	std::shared_ptr<SwapChain> swapChain;		//スワップチェイン
	std::shared_ptr<Descriptor> descriptor;		//デスクリプタ
	std::shared_ptr<RenderTarget> renderTarget;	//レンダーターゲット
	std::shared_ptr<Sampler> sampler;			//サンプラ
	std::shared_ptr<DescriptorRange> dRange;	//デスクリプタレンジ
	std::shared_ptr<Parameter> parameter;		//パラメータ
	std::shared_ptr<RootSignature> root;		//ルートシグネチャ
	std::shared_ptr<Fence> fence;				//フェンス
};

