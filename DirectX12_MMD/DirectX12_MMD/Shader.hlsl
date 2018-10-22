//テクスチャ
Texture2D<float4> tex   : register(t0); //通常テクスチャ
//Texture2D<float4> sph   : register(t1); //乗算テクスチャ
//Texture2D<float4> spa   : register(t2); //加算テクスチャ
//Texture2D<float4> toon  : register(t3); //トゥーンテクスチャ

SamplerState smp : register(s0);//サンプラ

//カメラ
cbuffer wvp : register(b0)
{
    float4x4 world;         //ワールド行列
    float4x4 view;          //ビュー行列
    float4x4 projection;    //プロジェクション行列
};

//マテリアル
cbuffer material : register(b1)
{
    float4  diffuse;    //減衰色
    float4  specular;   //光沢色
    float4  ambient;    //環境色
    bool    texFlag;    //テクスチャありなしフラグ
};

struct Out
{
    float4 svpos    : SV_POSITION;
    float4 pos      : POSITION;
    float3 normal   : NORMAL;
    float2 uv       : TEXCOORD;
};

//VertexShader
Out BasicVS(float4 pos : POSITION, float4 normal : NORMAL, float2 uv : TEXCOORD)
{
    Out o;
	//ワールドビュープロジェクション
    float4x4 vp = mul(projection, view);
    pos = mul(mul(vp, world), pos);

    o.svpos = pos;
    o.pos = pos;
    o.normal = mul(world, normal);
    o.uv = uv;
    return o;
}

//PixelShader
float4 BasicPS(Out o) : SV_TARGET
{
	//視点
    float3 eye = (0.0f, 10.0f, -15.0f);
    //視線
    float3 ray = normalize(eye - o.pos.xyz);
	//ライト
    float3 light = normalize(float3(-1, 1, -1));
    //反射ベクトル
    float3 mirror = reflect(-light, o.normal);

    float spec = saturate(dot(mirror, ray));

    spec = pow(spec, specular.a);

    float brightness = saturate(dot(light, o.normal.xyz)) * rcp(3.14f); //rcp : 正規化ランバート

    float3 color = tex.Sample(smp, o.uv).rgb * saturate(diffuse.rgb * brightness + specular.rgb * spec + ambient.rgb);

    return float4(color, ambient.a);
}
