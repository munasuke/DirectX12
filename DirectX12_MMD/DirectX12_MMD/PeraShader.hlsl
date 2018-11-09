//テクスチャ
Texture2D<float4> tex   : register(t0); //通常テクスチャ
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

//ポスタリゼーション
float4 Posterization(float4 texColor)
{
    return float4(texColor.rgb - fmod(texColor.rgb, 0.25f), 1.0f);
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

    float b = dot(float3(0.298912f, 0.586611f, 0.114478f), 1 - ret.rgb);
    b = pow(b, power);

    return float4(b, b, b, 1.0f);
}

//ピクセルシェーダ
float4 BasicPS(Out o) : SV_TARGET
{
    float4 texColor = tex.Sample(smp, o.uv);
    
    //リソースのサイズを取得
    float w, h, level;
    tex.GetDimensions(0, w, h, level);

    float2 d = float2(1.0f / w, 1.0f / h);
    float4 ret = tex.Sample(smp, o.uv);

    //return Posterization(texColor);
    //return AveragingFilter(ret, d, o.uv);
    //return GaussianFilter(ret, d, o.uv);
    return ExtractOutline(ret, d, o.uv, 6.0f);
}
