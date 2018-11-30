cbuffer wvp : register(b0)
{
    float4x4 world;         //ワールド行列
    float4x4 view;          //ビュー行列
    float4x4 projection;    //プロジェクション行列
    float4x4 lvp;           //ライトビュー行列
};

struct Out
{
    float4 svpos    : SV_Position;
    float3 normal   : NORMAL;
    float2 uv       : TEXCOORD;
    float4 color    : COLOR;
};

Out VS(float4 pos : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD, float4 color : COLOR)
{
    float4x4 vp     = mul(projection, view);
    float4x4 wvp    = mul(vp, world);

    Out o;
    o.svpos     = mul(wvp, pos);
    o.normal    = normal;
    o.uv        = uv;
    o.color     = color;

    return o;
}

float4 PS(Out o) : SV_TARGET
{
    return o.color;
}