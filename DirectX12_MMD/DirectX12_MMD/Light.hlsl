cbuffer wvp : register(b0)
{
    float4x4 world;         //���[���h�s��
    float4x4 view;          //�r���[�s��
    float4x4 projection;    //�v���W�F�N�V�����s��
    float4x4 lvp;           //���C�g�r���[�s��
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