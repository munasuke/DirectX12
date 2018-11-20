cbuffer wvp : register(b0)
{
    float4x4 world; //ワールド行列
    float4x4 view; //ビュー行列
    float4x4 projection; //プロジェクション行列
};

struct Out
{
    float4 svpos    : SV_Position;
    float3 normal   : NORMAL;
    float2 uv       : TEXCOORD;
};

Out VS(float3 pos : POSITION, float3 norm : NORMAL, float2 uv : TEXCOORD)
{
    Out o;
    o.svpos     = float4(pos, 1.0f);
    o.normal    = norm;
    o.uv        = uv;

    return o;
}

float4 PS(Out o) : SV_TARGET
{
    return float4(1.0f, 1.0f, 1.0f, 1.0f);

}