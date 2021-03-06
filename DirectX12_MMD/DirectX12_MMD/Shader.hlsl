//テクスチャ
Texture2D<float4> tex   : register(t0); //通常テクスチャ
Texture2D<float4> sph   : register(t1); //乗算テクスチャ
Texture2D<float4> spa   : register(t2); //加算テクスチャ
Texture2D<float4> toon  : register(t3); //トゥーンテクスチャ

//サンプラ
SamplerState smp : register(s0);

//カメラ
cbuffer wvp : register(b0)
{
    float4x4 world;         //ワールド行列
    float4x4 view;          //ビュー行列
    float4x4 projection;    //プロジェクション行列
    float4x4 lvp;           //ライトビュー行列
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

//入力
struct In
{
    float4      pos     : POSITION;
    float4      normal  : NORMAL;
    float2      uv      : TEXCOORD;
    min16uint2  boneno  : BONENO;
    min16uint2  weight  : WEIGHT;
    uint        iid     : SV_InstanceID;
};

//出力
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
Out BasicVS(In i)
{
    //出力変数
    Out o;

	//ワールドビュープロジェクション
    float4x4 vp = mul(projection, view);
    float4x4 wvp = mul(vp, world);

    //ウェイト
    float w = i.weight / 100.0f;

    //ボーンの影響度
    matrix boneImpact = boneMatrix[i.boneno.x] * w + boneMatrix[i.boneno.y] * (1 - w);
    i.pos = mul(boneImpact, i.pos);

    o.pos       = mul(world, i.pos);
    o.svpos     = mul(wvp, i.pos);
    o.normal    = mul(world, i.normal);
    o.uv        = i.uv;
    o.weight    = float2(w, 1 - w);

    return o;
}

struct POutput
{
    float4 col : SV_Target0;    //色
    float4 hbr : SV_Target1;    //高輝度
};

//スフィアUVの作成
float2 CreateSphereUV(float3 ray, float3 normal)
{
    //上ベクトル
    float3 upVec = float3(0.0f, 1.0f, 0.0f);

    //右ベクトルを計算
    float3 rightVec = normalize(cross(upVec, ray));

    //視線ベクトルに対しての上ベクトルを計算
    upVec = normalize(cross(ray, rightVec));

    //スフィアUV計算
    float2 uv = float2(dot(normal, rightVec), dot(normal, upVec));

    //-1〜1を0〜1に変換
    uv = float2(0.5f, -0.5f) * (uv + float2(1.0f, -1.0f));

    return uv;
}

//スペキュラ生成
float3 CreateSpecular(float3 light, float3 normal, float3 rray)
{
    //ライト反射ベクトル
    float3 mirror = reflect(-light, normal);

    //スペキュラ
    float spec = saturate(dot(mirror, rray));

    //スペキュラの強さ
    spec = pow(spec,1);

    return spec;
}

//ピクセルシェーダ
POutput BasicPS(Out o)
{
	//視点
    const float3 eye = float3(0.0f, 15.0f, -40.0f);

    //視線ベクトル
    float3 ray = o.pos.xyz - eye;

    //逆視線ベクトル
    float3 rray = -ray;

	//ライト
    const float3 light = float3(-1.0f, 1.0f, -1.0f);
    
    //円周率
    const float pi = 3.14159265359f;

    //明るさ
    float brightness = saturate(dot(light, o.normal)); //rcp : 正規化ランバート
    brightness = saturate(1 - acos(brightness) / pi);

    //スフィアUV取得
    float2 spuv = CreateSphereUV(ray, o.normal);

    //スペキュラ取得
    float3 spec = CreateSpecular(light, o.normal, rray);

    //トゥーン
    float4 tToon = toon.Sample(smp, float2(0.0f, 1.0f - brightness));

    //テクスチャカラー（通常*乗算+加算）
    float3 texColor = tex.Sample(smp, o.uv).rgb * sph.Sample(smp, spuv).rgb + spa.Sample(smp, spuv).rgb;

    //マテリアルカラー
    float3 color = saturate(tToon.rgb * diffuse.rgb * brightness + specular.rgb * spec + ambient.rgb);

    POutput output;
    //通常
    output.col = float4(pow(texColor * color, 2.2f), diffuse.a);
    //高輝度
    output.hbr = float4(o.uv.xy, 0.5f, 1.0f);

    return output;
}