//テクスチャ
Texture2D<float4> tex   : register(t0); //通常テクスチャ
//サンプラ
SamplerState smp        : register(s0);

struct Out
{
    float4 svpos    : SV_POSITION;
    float2 uv       : TEXCOORD;
};

//VertexShader
Out BasicVS(float4 pos : POSITION, float2 uv : TEXCOORD)
{
    Out o;
    o.svpos = pos;
    o.uv    = uv;
    return o;
}

//PixelShader
float4 BasicPS(Out o) : SV_TARGET
{
    return tex.Sample(smp, o.uv);
}
