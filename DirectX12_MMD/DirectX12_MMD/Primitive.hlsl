Texture2D<float> tex : register(t0); //�[�x�e�N�X�`��
SamplerState smp : register(s0);

cbuffer wvp : register(b0)
{
    float4x4 world;         //���[���h�s��
    float4x4 view;          //�r���[�s��
    float4x4 projection;    //�v���W�F�N�V�����s��
    float4x4 lvp;           //���C�g�r���[�s��
};

struct Out
{
    float4 pos      : POSITION;
    float4 svpos    : SV_Position;
    float4 lightPos : LIGHTPOS;
    float3 normal   : NORMAL;
    float2 uv       : TEXCOORD;
    float4 color    : COLOR;
};

Out VS(float4 pos : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD, float4 color : COLOR)
{
    float4x4 vp     = mul(projection, view);
    float4x4 wvp    = mul(vp, world);

    Out o;
    o.pos       = mul(world, pos);
    o.svpos     = mul(wvp, pos);
    o.lightPos  = mul(lvp, o.pos);
    o.normal    = normal;
    o.uv        = uv;
    o.color     = color;

    return o;
}

float4 PS(Out o) : SV_TARGET
{
    float2 shadowUV = (float2(1.0f, -1.0f) + o.lightPos.xy) * float2(0.5f, -0.5f);
    float dep = pow(tex.Sample(smp, shadowUV), 200);
    return float4(o.color.rgb * dep, o.color.a);
}