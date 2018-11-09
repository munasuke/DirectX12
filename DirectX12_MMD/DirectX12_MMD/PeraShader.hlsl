//�e�N�X�`��
Texture2D<float4> tex   : register(t0); //�ʏ�e�N�X�`��
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

//�|�X�^���[�[�V����
float4 Posterization(float4 texColor)
{
    return float4(texColor.rgb - fmod(texColor.rgb, 0.25f), 1.0f);
}

//���ω��ڂ����i9�ߖT�j
float4 AveragingFilter(float4 ret, float2 d, float2 uv)
{
    ret = float4(0.0f, 0.0f, 0.0f, 0.0f);

    //����8�s�N�Z���̕��ς����
    ret += tex.Sample(smp, uv + float2(-2 * d.x, 2 * d.y)) / 9.0f;
    ret += tex.Sample(smp, uv + float2(2 * d.x, 2 * d.y)) / 9.0f;
    ret += tex.Sample(smp, uv + float2(0, 2 * d.y)) / 9.0f;

    ret += tex.Sample(smp, uv + float2(-2 * d.x, 0)) / 9.0f;
    ret += tex.Sample(smp, uv) / 9.0f;
    ret += tex.Sample(smp, uv + float2(2 * d.x, 0)) / 9.0f;

    ret += tex.Sample(smp, uv + float2(-2 * d.x, -2 * d.y)) / 9.0f;
    ret += tex.Sample(smp, uv + float2(2 * d.x, -2 * d.y)) / 9.0f;
    ret += tex.Sample(smp, uv + float2(0, -2 * d.y)) / 9.0f;

    //���ς���������̂���f�l�Ƃ���
    return ret;
}

//�K�E�V�A���ڂ���
float4 GaussianFilter(float4 ret, float2 d, float2 uv)
{
    ret = ret * 36 / 256;

    d.x *= 2;
    d.y *= 2; 

    //���̃s�N�Z����8�ߖT�̃s�N�Z���l�����Z 
    ret += tex.Sample(smp, uv + float2(-2 * d.x, 2 * d.y)) * 1 / 256; 
    ret += tex.Sample(smp, uv + float2(-1 * d.x, 2 * d.y)) * 4 / 256; 
    ret += tex.Sample(smp, uv + float2( 0 * d.x, 2 * d.y)) * 6 / 256; 
    ret += tex.Sample(smp, uv + float2( 1 * d.x, 2 * d.y)) * 4 / 256; 
    ret += tex.Sample(smp, uv + float2( 2 * d.x, 2 * d.y)) * 1 / 256;
    
    ret += tex.Sample(smp, uv + float2(-2 * d.x, 1 * d.y)) * 4  / 256; 
    ret += tex.Sample(smp, uv + float2(-1 * d.x, 1 * d.y)) * 16 / 256; 
    ret += tex.Sample(smp, uv + float2( 0 * d.x, 1 * d.y)) * 24 / 256; 
    ret += tex.Sample(smp, uv + float2( 1 * d.x, 1 * d.y)) * 16 / 256; 
    ret += tex.Sample(smp, uv + float2( 2 * d.x, 1 * d.y)) * 4  / 256;
    
    ret += tex.Sample(smp, uv + float2(-2 * d.x, 0 * d.y)) * 6  / 256; 
    ret += tex.Sample(smp, uv + float2(-1 * d.x, 0 * d.y)) * 24 / 256;

    //���Ɍv�Z�ς� 
    ret += tex.Sample(smp, uv + float2( 1 * d.x,  0 * d.y)) * 24 / 256; 
    ret += tex.Sample(smp, uv + float2( 2 * d.x,  0 * d.y)) *  6 / 256;
    
    ret += tex.Sample(smp, uv + float2(-2 * d.x, -1 * d.y)) *  4 / 256; 
    ret += tex.Sample(smp, uv + float2(-1 * d.x, -1 * d.y)) * 16 / 256; 
    ret += tex.Sample(smp, uv + float2( 0 * d.x, -1 * d.y)) * 24 / 256; 
    ret += tex.Sample(smp, uv + float2( 1 * d.x, -1 * d.y)) * 16 / 256; 
    ret += tex.Sample(smp, uv + float2( 2 * d.x, -1 * d.y)) *  4 / 256;
    
    ret += tex.Sample(smp, uv + float2(-2 * d.x, -2 * d.y)) *  1 / 256; 
    ret += tex.Sample(smp, uv + float2(-1 * d.x, -2 * d.y)) *  4 / 256; 
    ret += tex.Sample(smp, uv + float2( 0 * d.x, -2 * d.y)) *  6 / 256; 
    ret += tex.Sample(smp, uv + float2( 1 * d.x, -2 * d.y)) *  4 / 256; 
    ret += tex.Sample(smp, uv + float2( 2 * d.x, -2 * d.y)) *  1 / 256;

    return ret;
}

//�֊s�����o
float4 ExtractOutline(float4 ret, float2 d, float2 uv, float power = 1.0f)
{
    ret = ret * 4 -
            tex.Sample(smp, uv + float2(-d.x, 0)) - tex.Sample(smp, uv + float2(d.x, 0)) -
            tex.Sample(smp, uv + float2(0, d.y)) - tex.Sample(smp, uv + float2(0, -d.y));

    float b = dot(float3(0.298912f, 0.586611f, 0.114478f), 1 - ret.rgb);
    b = pow(b, power);

    return float4(b, b, b, 1.0f);
}

//�s�N�Z���V�F�[�_
float4 BasicPS(Out o) : SV_TARGET
{
    float4 texColor = tex.Sample(smp, o.uv);
    
    //���\�[�X�̃T�C�Y���擾
    float w, h, level;
    tex.GetDimensions(0, w, h, level);

    float2 d = float2(1.0f / w, 1.0f / h);
    float4 ret = tex.Sample(smp, o.uv);

    //return Posterization(texColor);
    //return AveragingFilter(ret, d, o.uv);
    //return GaussianFilter(ret, d, o.uv);
    return ExtractOutline(ret, d, o.uv, 6.0f);
}
