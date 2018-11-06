//テクスチャ
Texture2D<float4> tex   : register(t0); //通常テクスチャ
Texture2D<float4> sph   : register(t1); //乗算テクスチャ
Texture2D<float4> spa   : register(t2); //加算テクスチャ
Texture2D<float4> toon  : register(t3); //トゥーンテクスチャ

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

//ボーン
cbuffer bones : register(b2)
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

//VertexShader
Out BasicVS(float4 pos : POSITION, float4 normal : NORMAL, float2 uv : TEXCOORD, min16uint2 boneno : BONENO, min16uint weight : WEIGHT)
{
    Out o;
	//ワールドビュープロジェクション
    float4x4 vp = mul(projection, view);
    float4x4 wvp = mul(vp, world);

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

//PixelShader
float4 BasicPS(Out o) : SV_TARGET
{
	//視点
    float3 eye = float3(0.0f, 15.0f, -40.0f);

    //逆視線ベクトル
    float3 rray = eye - o.pos.xyz;

    //視線ベクトル
    float3 ray = o.pos.xyz - eye;

    //上ベクトル
    float3 upVec = float3(0.0f, 1.0f, 0.0f);

    //右ベクトルを計算
    float3 rightVec = normalize(cross(upVec, ray));

    //視線ベクトルに対しての上ベクトルを計算
    upVec = normalize(cross(ray, rightVec));

    //スフィアUV計算
    float2 uv = float2(dot(o.normal.xyz, rightVec), dot(o.normal.xyz, upVec));

    //-1〜1を0〜1に変換
    uv = float2(0.5f, -0.5f) * (uv + float2(1.0f, -1.0f));

	//ライト
    float3 light = normalize(float3(-1.0f, 1.0f, -1.0f));

    //ライト反射ベクトル
    float3 mirror = reflect(-light, o.normal);

    //スペキュラ
    float spec = saturate(dot(mirror, rray));

    //スペキュラの強さ
    spec = pow(spec, specular.a);

    //円周率
    float pi = 3.14159265359;

    //明るさ
    float brightness = saturate(dot(light, o.normal.xyz)); //rcp : 正規化ランバート
    brightness = saturate(1 - acos(brightness) / pi);

    //トゥーン
    float4 tToon = toon.Sample(smp, float2(0.0f, 1.0f - brightness));

    //テクスチャカラー（通常*乗算+加算）
    float3 texColor = tex.Sample(smp, o.uv).rgb * sph.Sample(smp, uv).rgb + spa.Sample(smp, uv).rgb;

    //マテリアルカラー
    float3 color = saturate(tToon.rgb * diffuse.rgb * brightness + specular.rgb * spec + ambient.rgb);

    //return float4(pow(color.r, 2.2f), pow(color.g, 2.2f), pow(color.b, 2.2f), diffuse.a);

    return float4(pow(texColor * color, 2.2f), diffuse.a);
}
