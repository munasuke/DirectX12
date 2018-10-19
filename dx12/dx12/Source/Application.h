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

	void Initialize();	//初期化
	void Run();			//メインループ
	void Terminate();	//終了

	~Application();
private:
	Application();							//生成禁止
	Application(const Application&);		//コピー禁止
	void operator=(const Application&) {};	//代入禁止

	std::shared_ptr<Window>				window;			//ウィンドウ
	std::shared_ptr<Device>				device;			//デバイス
	std::shared_ptr<Command>			command;		//コマンド
	std::shared_ptr<SwapChain>			swapChain;		//スワップチェイン
	std::shared_ptr<Descriptor>			descriptor;		//デスクリプタ
	std::shared_ptr<RenderTarget>		renderTarget;	//レンダーターゲット
	std::shared_ptr<Sampler>			sampler;		//サンプラ
	std::shared_ptr<RootSignature>		root;			//ルートシグネチャ
	std::shared_ptr<Fence>				fence;			//フェンス
	std::shared_ptr<Vertex>				vertex;			//頂点バッファ
	//std::shared_ptr<TextureResource>	tex;			//テクスチャリソース
	//std::shared_ptr<ShaderResourceView> srv;			//シェーダリソースビュー
	std::shared_ptr<BmpLoader>			bmp;			//BMP
	std::shared_ptr<ImageLoader>		imageL;			//画像ロード
	std::shared_ptr<ShaderLoader>		shader;			//シェーダ
	std::shared_ptr<PipelineState>		pipline;		//パイプラインステートオブジェクト
	std::shared_ptr<ViewPort>			viewPort;		//ビューポート
	//std::shared_ptr<ConstantBuffer>		cb;				//コンスタントバッファ
	std::shared_ptr<PMDLoader>			pmd;			//PMD
	std::shared_ptr<Index>				index;			//インデックス
	std::shared_ptr<DepthStencilBuffer>	depth;			//深度バッファ
	std::shared_ptr<Model>				model;			//モデル
	std::shared_ptr<Camera>				camera;			//カメラ
};

