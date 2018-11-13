//�e�N�X�`��
Texture2D<float4> tex   : register(t0);
//�T���v��
SamplerState smp        : register(s0);

struct Out
{
    float4 svpos    : SV_POSITION;
    float2 uv       : TEXCOORD;
};

//���_�V�F�[�_
Out BasicVS(float4 pos : POSITION, float2 uv : TEXCOORD)
{
    Out o;
    o.svpos = pos;
    o.uv    = uv;

    return o;
}

//�s�N�Z���V�F�[�_
float4 BasicPS(Out o) : SV_TARGET
{
    //���\�[�X�J���[
    float4 texColor = tex.Sample(smp, o.uv);
    
    return texColor;
}
