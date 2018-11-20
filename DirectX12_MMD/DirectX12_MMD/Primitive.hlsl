cbuffer wvp : register(b0)
{
    float4x4 world; //���[���h�s��
    float4x4 view; //�r���[�s��
    float4x4 projection; //�v���W�F�N�V�����s��
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