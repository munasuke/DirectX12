//テクスチャ
Texture2D<float4> tex : register(t0);

//カメラ
cbuffer wvp : register(b0)
{
    float4x4 world;         //ワールド行列
    float4x4 view;          //ビュー行列
    float4x4 projection;    //プロジェクション行列
    float4x4 lvp;           //ライトビュー行列
};

//ボーン
cbuffer bones : register(b1)
{
    matrix boneMatrix[512];
};

struct Out
{
    float4 svpos    : SV_POSITION;
    float4 pos      : POSITION;
    float3 normal   : NORMAL0;
    float2 uv       : TEXCOORD;
    float2 bone     : BONENO;
    float2 weight   : WEIGHT;
};

//頂点シェーダ
Out VS(float4 pos : POSITION, float4 normal : NORMAL, float2 uv : TEXCOORD, min16uint2 boneno : BONENO, min16uint weight : WEIGHT)
{
    Out o;

	//ワールドビュープロジェクション
    float4x4 vp     = mul(projection, view);
    float4x4 wvp    = mul(vp, world);

    //ウェイト
    float w = weight / 100.0f;

    //ボーンの影響度
    matrix boneImpact = boneMatrix[boneno.x] * w + boneMatrix[boneno.y] * (1 - w);
    pos = mul(boneImpact, pos);

    o.pos       = mul(world, pos);
    o.svpos     = mul(wvp, pos);
    o.normal    = mul(world, normal);
    o.uv        = uv;
    o.weight    = float2(w, 1 - w);

    return o;
}


//ピクセルシェーダ
float4 PS(Out o) : SV_TARGET
{
    return float4(0.0f, 0.0f, 0.0f, 1.0f);
}