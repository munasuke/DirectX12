//テクスチャ
Texture2D<float4> tex   : register(t0);
//サンプラ
SamplerState smp        : register(s0);

struct Out
{
    float4 svpos    : SV_POSITION;
    float2 uv       : TEXCOORD;
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
float4 Sepia(float3 rgb)
{
    float v = dot(value, rgb);
    return float4(v * 0.5f, v * 0.5f, v * 0.7f, 1.0f);
    return float4(v * 0.9f, v * 0.7f, v * 0.4f, 1.0f);
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
    ret += tex.Sample(smp, uv + float2(-2 * d.x, 2 * d.y)) * 1 / 256;
    ret += tex.Sample(smp, uv + float2(-1 * d.x, 2 * d.y)) * 4 / 256; 
    ret += tex.Sample(smp, uv + float2( 0 * d.x, 2 * d.y)) * 6 / 256; 
    ret += tex.Sample(smp, uv + float2( 1 * d.x, 2 * d.y)) * 4 / 256; 
    ret += tex.Sample(smp, uv + float2( 2 * d.x, 2 * d.y)) * 1 / 256;
    
    ret += tex.Sample(smp, uv + float2(-2 * d.x, 1 * d.y)) * 4  / 256; 
    ret += tex.Sample(smp, uv + float2(-1 * d.x, 1 * d.y)) * 16 / 256; 
    ret += tex.Sample(smp, uv + float2( 0 * d.x, 1 * d.y)) * 24 / 256; 
    ret += tex.Sample(smp, uv + float2( 1 * d.x, 1 * d.y)) * 16 / 256; 
    ret += tex.Sample(smp, uv + float2( 2 * d.x, 1 * d.y)) * 4  / 256;
    
    ret += tex.Sample(smp, uv + float2(-2 * d.x, 0 * d.y)) * 6  / 256; 
    ret += tex.Sample(smp, uv + float2(-1 * d.x, 0 * d.y)) * 24 / 256;

    //既に計算済み 
    ret += tex.Sample(smp, uv + float2( 1 * d.x,  0 * d.y)) * 24 / 256; 
    ret += tex.Sample(smp, uv + float2( 2 * d.x,  0 * d.y)) *  6 / 256;
    
    ret += tex.Sample(smp, uv + float2(-2 * d.x, -1 * d.y)) *  4 / 256; 
    ret += tex.Sample(smp, uv + float2(-1 * d.x, -1 * d.y)) * 16 / 256; 
    ret += tex.Sample(smp, uv + float2( 0 * d.x, -1 * d.y)) * 24 / 256; 
    ret += tex.Sample(smp, uv + float2( 1 * d.x, -1 * d.y)) * 16 / 256; 
    ret += tex.Sample(smp, uv + float2( 2 * d.x, -1 * d.y)) *  4 / 256;
    
    ret += tex.Sample(smp, uv + float2(-2 * d.x, -2 * d.y)) *  1 / 256; 
    ret += tex.Sample(smp, uv + float2(-1 * d.x, -2 * d.y)) *  4 / 256; 
    ret += tex.Sample(smp, uv + float2( 0 * d.x, -2 * d.y)) *  6 / 256; 
    ret += tex.Sample(smp, uv + float2( 1 * d.x, -2 * d.y)) *  4 / 256; 
    ret += tex.Sample(smp, uv + float2( 2 * d.x, -2 * d.y)) *  1 / 256;

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

//ランダム
float Rand(float2 co)
{
    return frac(sin(dot(co, float2(12.9898, 78.233))) * 43758.5453);
}

//お風呂場のガラス（曇りガラス）
float4 FrostedGlass(float2 uv, float2 wh)
{
    float radius = 5.0f;
    float2 _uv = float2((uv.x * wh.x) + Rand(uv) * radius * 2.0f - radius, 
                    (uv.y * wh.y) + Rand(float2(uv.y, uv.x)) * radius * 2.0f - radius);

    return float4(tex.Sample(smp, float2(_uv.x / wh.x, _uv.y / wh.y)));
}

//うずまきナルト
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

//ピクセルシェーダ
float4 BasicPS(Out o) : SV_TARGET
{
    //リソースカラー
    float4 texColor = tex.Sample(smp, o.uv);
    
    //リソースのサイズを取得
    float w, h, level;
    tex.GetDimensions(0, w, h, level);

    float2 d = float2(1.0f / w, 1.0f / h);
    float4 ret = tex.Sample(smp, o.uv);

    //return texColor;
    //return Monochrome(texColor.rgb);
    return Sepia(ExtractOutline(FrostedGlass(o.uv, float2(w, h)), d, o.uv, 4.0f).rgb);
    //return Sepia(texColor.rgb);
    //return Posterization(texColor.rgb);
    //return AveragingFilter(ret, d, o.uv);
    //return GaussianFilter(ret, d, o.uv);
    //return ExtractOutline(ret, d, o.uv, 6.0f);
    return FrostedGlass(o.uv, float2(w, h));
    return Spiral(o.uv, float2(w, h), 200.0f, 8.0f);
}
