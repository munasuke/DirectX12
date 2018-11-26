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
};

Out VS(float4 pos : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD)
{
    float4x4 vp = mul(projection, view);
    float4x4 wvp = mul(vp, world);

    Out o;
    o.svpos = mul(wvp, pos);
    o.normal = normal;
    o.uv = uv;

    return o;
}

float4 PS(Out o) : SV_TARGET
{
    float3 rgb = float3(1.0f, 1.0f, 1.0f);

    return float4(rgb, 1.0f);
}