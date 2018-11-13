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

//ピクセルシェーダ
float4 BasicPS(Out o) : SV_TARGET
{
    //リソースカラー
    float4 texColor = tex.Sample(smp, o.uv);
    
    return texColor;
}
