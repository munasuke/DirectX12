#define FXAA_PC 1
#define FXAA_HLSL_5 1
#include "FXAA.hlsl"

//テクスチャ
Texture2D<float4> tex   : register(t0);
Texture2D<float> depth   : register(t1);
Texture2D<float> bloom   : register(t2);
//サンプラ
SamplerState smp        : register(s0);

struct Out
{
    float4 svpos    : SV_POSITION;
    float2 uv       : TEXCOORD0;
};

//頂点シェーダ
Out BasicVS(float4 pos : POSITION, float2 uv : TEXCOORD)
{
    Out o;
    o.svpos = pos;
    o.uv    = uv;

    return o;
}


//定数
//輝度値計算用
#define value  float3(0.298912f, 0.586611f, 0.114478f)

//モノクロ
float4 Monochrome(float3 rgb)
{
    float m = dot(value, rgb);
    return float4(m, m, m, 1.0f);
}

//セピア調
float3 Sepia(float3 rgb, float3 sepiaCol)
{
    float v = dot(value, rgb);
    return v * sepiaCol;
    return v * 0.9f, v * 0.7f, v * 0.4f;
}

//ポスタリゼーション
float4 Posterization(float3 rgb)
{
    return float4(rgb - fmod(rgb, 0.25f), 1.0f);
}

//平均化ぼかし（9近傍）
float4 AveragingFilter(float4 ret, float2 d, float2 uv)
{
    ret = float4(0.0f, 0.0f, 0.0f, 0.0f);

    //周囲8ピクセルの平均を取る
    ret += tex.Sample(smp, uv + float2(-2 * d.x, 2 * d.y)) / 9.0f;
    ret += tex.Sample(smp, uv + float2(2 * d.x, 2 * d.y)) / 9.0f;
    ret += tex.Sample(smp, uv + float2(0, 2 * d.y)) / 9.0f;

    ret += tex.Sample(smp, uv + float2(-2 * d.x, 0)) / 9.0f;
    ret += tex.Sample(smp, uv) / 9.0f;
    ret += tex.Sample(smp, uv + float2(2 * d.x, 0)) / 9.0f;

    ret += tex.Sample(smp, uv + float2(-2 * d.x, -2 * d.y)) / 9.0f;
    ret += tex.Sample(smp, uv + float2(2 * d.x, -2 * d.y)) / 9.0f;
    ret += tex.Sample(smp, uv + float2(0, -2 * d.y)) / 9.0f;

    //平均を取ったものを画素値とする
    return ret;
}

//ガウシアンぼかし
float4 GaussianFilter(float4 ret, float2 d, float2 uv)
{
    ret = ret * 36 / 256;

    d.x *= 2;
    d.y *= 2; 

    //今のピクセルに8近傍のピクセル値を加算 
    //1行目
    ret += tex.Sample(smp, uv + float2(-2 * d.x, -2 * d.y)) *  1 / 256; 
    ret += tex.Sample(smp, uv + float2(-1 * d.x, -2 * d.y)) *  4 / 256; 
    ret += tex.Sample(smp, uv + float2( 0 * d.x, -2 * d.y)) *  6 / 256; 
    ret += tex.Sample(smp, uv + float2( 1 * d.x, -2 * d.y)) *  4 / 256; 
    ret += tex.Sample(smp, uv + float2( 2 * d.x, -2 * d.y)) *  1 / 256;

    //2行目
    ret += tex.Sample(smp, uv + float2(-2 * d.x, -1 * d.y)) *  4 / 256; 
    ret += tex.Sample(smp, uv + float2(-1 * d.x, -1 * d.y)) * 16 / 256; 
    ret += tex.Sample(smp, uv + float2( 0 * d.x, -1 * d.y)) * 24 / 256; 
    ret += tex.Sample(smp, uv + float2( 1 * d.x, -1 * d.y)) * 16 / 256; 
    ret += tex.Sample(smp, uv + float2( 2 * d.x, -1 * d.y)) *  4 / 256;
    
    //3行目
    ret += tex.Sample(smp, uv + float2( -2 * d.x, 0 * d.y)) *  6 / 256; 
    ret += tex.Sample(smp, uv + float2( -1 * d.x, 0 * d.y)) * 24 / 256;
    ret += tex.Sample(smp, uv + float2( 1 * d.x,  0 * d.y)) * 24 / 256; 
    ret += tex.Sample(smp, uv + float2( 2 * d.x,  0 * d.y)) *  6 / 256;
    
    //4行目
    ret += tex.Sample(smp, uv + float2(-2 * d.x, 1 * d.y)) *  4 / 256; 
    ret += tex.Sample(smp, uv + float2(-1 * d.x, 1 * d.y)) * 16 / 256; 
    ret += tex.Sample(smp, uv + float2( 0 * d.x, 1 * d.y)) * 24 / 256; 
    ret += tex.Sample(smp, uv + float2( 1 * d.x, 1 * d.y)) * 16 / 256; 
    ret += tex.Sample(smp, uv + float2( 2 * d.x, 1 * d.y)) *  4 / 256;
    
    //5行目
    ret += tex.Sample(smp, uv + float2(-2 * d.x, 2 * d.y)) * 1 / 256;
    ret += tex.Sample(smp, uv + float2(-1 * d.x, 2 * d.y)) * 4 / 256; 
    ret += tex.Sample(smp, uv + float2( 0 * d.x, 2 * d.y)) * 6 / 256; 
    ret += tex.Sample(smp, uv + float2( 1 * d.x, 2 * d.y)) * 4 / 256; 
    ret += tex.Sample(smp, uv + float2( 2 * d.x, 2 * d.y)) * 1 / 256;
    
    return ret;
}

//エンボス
float4 Emboss(float4 ret, float2 uv, float2 d)
{
    /*
        2  1  0
        1  1 -1
        0 -1 -1
    */
    ret += tex.Sample(smp, uv + float2(-d.x, -d.y)) * 2.0f;
    ret += tex.Sample(smp, uv + float2(0.0f, -d.y)) * 1.0f;
    ret += tex.Sample(smp, uv + float2(-d.x, 0.0f)) * 1.0f;
    ret += tex.Sample(smp, uv + float2(d.x, 0.0f)) * -1.0f;
    ret += tex.Sample(smp, uv + float2(0.0f, d.y)) * -1.0f;
    ret += tex.Sample(smp, uv + float2(d.x, d.y)) * -2.0f;

    return ret;
}

//シャープネス
float4 Sharpness(float4 ret, float2 uv, float2 d)
{
    ret += tex.Sample(smp, uv + float2(0.0f, -d.y)) * -1.0f;
    ret += tex.Sample(smp, uv + float2(-d.x, 0.0f)) * -1.0f;
    ret += tex.Sample(smp, uv) * 5.0f;
    ret += tex.Sample(smp, uv + float2(d.x, 0.0f)) * -1.0f;
    ret += tex.Sample(smp, uv + float2(0.0f, d.y)) * -1.0f;

    return ret;
}

//輪郭線抽出
float4 ExtractOutline(float4 ret, float2 d, float2 uv, float power = 1.0f)
{
    ret = ret * 4 -
            tex.Sample(smp, uv + float2(-d.x, 0)) - tex.Sample(smp, uv + float2(d.x, 0)) -
            tex.Sample(smp, uv + float2(0, d.y)) - tex.Sample(smp, uv + float2(0, -d.y));

    float b = dot(value, 1 - ret.rgb);
    b = pow(b, power);

    return float4(1.0f - b, 1.0f - b, 1.0f - b, 1.0f);
}

/**
* @fn float Rand(float2 co)
* @brief 乱数を作る関数
* @param co テクスチャ
* @return float 生成された乱数を返す
*/
float Rand(float2 co)
{
    return frac(sin(dot(co, float2(12.9898, 78.233))) * 43758.5453);
}

/**
* @fn float4 FrostedGlass(float2 uv, float2 wh)
* @brief 曇りガラスを作る関数
* @param uv テクスチャのUV座標
* @param wh テクスチャの横幅と高さ
* @return float4 曇りガラスの出来たテクスチャを返す
* @details 周囲のピクセルからランダムにピクセルを取得する
*/
float4 FrostedGlass(float2 uv, float2 wh)
{
    float radius = 5.0f;
    float2 _uv = float2((uv.x * wh.x) + Rand(uv) * radius * 2.0f - radius, 
                    (uv.y * wh.y) + Rand(float2(uv.y, uv.x)) * radius * 2.0f - radius);

    return float4(tex.Sample(smp, float2(_uv.x / wh.x, _uv.y / wh.y)));
}

/**
* @fn float4 Spiral(float2 uv, float2 screen, float radius = 100.0f, float strength = 10.0f)
* @brief 渦巻きを作る関数
* @param uv テクスチャのUV座標
* @param screen 画面サイズ
* @param radius 渦の大きさ
* @param strength 渦の強さ
* @return float4 渦巻きの出来たテクスチャを返す
* @details 詳細な説明
*/
float4 Spiral(float2 uv, float2 screen, float radius = 100.0f, float strength = 10.0f)
{
    float2 centor = screen / 2.0f;
    float2 pos = (uv * screen) - centor;
    float len = length(pos);
    if (len >= radius)
    {
        return tex.Sample(smp, uv);
    }

    float spiral = min(max(1.0f - (len / radius), 0.0f), 1.0f) * strength;
    float2 xy = float2(pos.x * cos(spiral) - pos.y * sin(spiral), pos.x * sin(spiral) + pos.y * cos(spiral));

    return tex.Sample(smp, (xy + centor) / screen);
}

/**
* @fn float4 Fxaa(float2 uv, float2 size, float2 d, float alpha)
* @brief FXAAを適用する関数
* @param uv     テクスチャUV
* @param size   テクスチャ幅高さ
* @param d      1ピクセル分のサイズ
* @param alpha  テクスチャのアルファ値
* @return float4 FXAAを適用した画像
*/
float4 Fxaa(float2 uv, float2 size, float2 d, float alpha)
{
    FxaaTex InputFXAATex = { smp, tex };
    return float4(FxaaPixelShader(
        uv,                                     // FxaaFloat2 pos,
        FxaaFloat4(0.0f, 0.0f, size.x, size.y), // FxaaFloat4 fxaaConsolePosPos,
        InputFXAATex,                           // FxaaTex tex,
        InputFXAATex,                           // FxaaTex fxaaConsole360TexExpBiasNegOne,
        InputFXAATex,                           // FxaaTex fxaaConsole360TexExpBiasNegTwo,
        d,                                      // FxaaFloat2 fxaaQualityRcpFrame,
        FxaaFloat4(0.0f, 0.0f, 0.0f, 0.0f),     // FxaaFloat4 fxaaConsoleRcpFrameOpt,
        FxaaFloat4(0.0f, 0.0f, 0.0f, 0.0f),     // FxaaFloat4 fxaaConsoleRcpFrameOpt2,
        FxaaFloat4(0.0f, 0.0f, 0.0f, 0.0f),     // FxaaFloat4 fxaaConsole360RcpFrameOpt2,
        0.75f,                                  // FxaaFloat fxaaQualitySubpix,
        0.166f,                                 // FxaaFloat fxaaQualityEdgeThreshold,
        0.0833f,                                // FxaaFloat fxaaQualityEdgeThresholdMin,
        8.0f,                                   // FxaaFloat fxaaConsoleEdgeSharpness,
        0.125f,                                 // FxaaFloat fxaaConsoleEdgeThreshold,
        0.005f,                                 // FxaaFloat fxaaConsoleEdgeThresholdMin,
        FxaaFloat4(0.0f, 0.0f, 0.0f, 0.0f)      // FxaaFloat fxaaConsole360ConstDir,
    ).xyz, alpha);
}

//ブラー
float4 Blur(float2 uv, float diff = 0.01f)
{
    float3 color = tex.Sample(smp, uv - diff).rgb + tex.Sample(smp, uv + diff).rgb;
    return float4(color / 2, 1.0f);
}

//ピクセルシェーダ
float4 BasicPS(Out o) : SV_TARGET
{
    //リソースカラー
    float4 texColor = tex.Sample(smp, o.uv);

    //深度値
    float dep = pow(depth.Sample(smp, o.uv), 200);
    
    //リソースのサイズを取得
    float w, h;
    tex.GetDimensions(w, h);

    //1ピクセル分取得
    float2 d = float2(1.0f / w, 1.0f / h);

    //セピアカラー
    float3 sepia = float3(0.5f, 0.8f, 0.5f);
    
    //加工しない部分の指定
    if (o.uv.x + o.uv.y < 0.9f)
    {
        return Fxaa(o.uv, float2(w, h), d, texColor.a);
        //return texColor;
    }
    //return float4(dep, dep, dep, 1.0f);

    //half2 pos = o.uv * 2.0f - 1.0f;
    //half p = length(pos);
    //pos = (1.0f + 1.0f * p * p) / (1.0f + 2.0f * 1.0f) * pos;

    //return tex.Sample(smp, pos * 0.5f + 0.5f);

    //float2 tuv = float2(cos(o.uv.x) - o.uv.x, sin(o.uv.y) + o.uv.y);
    //return tex.Sample(smp, tuv);
    //return texColor;
    //return Monochrome(texColor.rgb);
    //return Emboss(texColor, o.uv, d);
    //return float4(Sepia(ExtractOutline(FrostedGlass(o.uv, float2(w, h)), d, o.uv, 6.0f).rgb, sepia), texColor.a);
    //return Sepia(texColor.rgb, sepia);
    //return Posterization(texColor.rgb);
    //return AveragingFilter(texColor, d, o.uv);
    //return GaussianFilter(texColor, d, o.uv);
    //return ExtractOutline(texColor, d, o.uv, 6.0f);
    return FrostedGlass(o.uv, float2(w, h));
    //return Spiral(o.uv, float2(w, h), 200.0f, 8.0f);
    return Fxaa(o.uv, float2(w, h), d, texColor.a);
    //return Blur(o.uv, 0.002f);
}
