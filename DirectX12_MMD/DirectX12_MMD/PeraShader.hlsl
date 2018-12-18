#define FXAA_PC 1
#define FXAA_HLSL_5 1
#include "FXAA.hlsl"

//�e�N�X�`��
Texture2D<float4> tex   : register(t0);
Texture2D<float> depth   : register(t1);
Texture2D<float> bloom   : register(t2);
//�T���v��
SamplerState smp        : register(s0);

struct Out
{
    float4 svpos    : SV_POSITION;
    float2 uv       : TEXCOORD0;
};

//���_�V�F�[�_
Out BasicVS(float4 pos : POSITION, float2 uv : TEXCOORD)
{
    Out o;
    o.svpos = pos;
    o.uv    = uv;

    return o;
}


//�萔
//�P�x�l�v�Z�p
#define value  float3(0.298912f, 0.586611f, 0.114478f)

//���m�N��
float4 Monochrome(float3 rgb)
{
    float m = dot(value, rgb);
    return float4(m, m, m, 1.0f);
}

//�Z�s�A��
float3 Sepia(float3 rgb, float3 sepiaCol)
{
    float v = dot(value, rgb);
    return v * sepiaCol;
    return v * 0.9f, v * 0.7f, v * 0.4f;
}

//�|�X�^���[�[�V����
float4 Posterization(float3 rgb)
{
    return float4(rgb - fmod(rgb, 0.25f), 1.0f);
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
    //1�s��
    ret += tex.Sample(smp, uv + float2(-2 * d.x, -2 * d.y)) *  1 / 256; 
    ret += tex.Sample(smp, uv + float2(-1 * d.x, -2 * d.y)) *  4 / 256; 
    ret += tex.Sample(smp, uv + float2( 0 * d.x, -2 * d.y)) *  6 / 256; 
    ret += tex.Sample(smp, uv + float2( 1 * d.x, -2 * d.y)) *  4 / 256; 
    ret += tex.Sample(smp, uv + float2( 2 * d.x, -2 * d.y)) *  1 / 256;

    //2�s��
    ret += tex.Sample(smp, uv + float2(-2 * d.x, -1 * d.y)) *  4 / 256; 
    ret += tex.Sample(smp, uv + float2(-1 * d.x, -1 * d.y)) * 16 / 256; 
    ret += tex.Sample(smp, uv + float2( 0 * d.x, -1 * d.y)) * 24 / 256; 
    ret += tex.Sample(smp, uv + float2( 1 * d.x, -1 * d.y)) * 16 / 256; 
    ret += tex.Sample(smp, uv + float2( 2 * d.x, -1 * d.y)) *  4 / 256;
    
    //3�s��
    ret += tex.Sample(smp, uv + float2( -2 * d.x, 0 * d.y)) *  6 / 256; 
    ret += tex.Sample(smp, uv + float2( -1 * d.x, 0 * d.y)) * 24 / 256;
    ret += tex.Sample(smp, uv + float2( 1 * d.x,  0 * d.y)) * 24 / 256; 
    ret += tex.Sample(smp, uv + float2( 2 * d.x,  0 * d.y)) *  6 / 256;
    
    //4�s��
    ret += tex.Sample(smp, uv + float2(-2 * d.x, 1 * d.y)) *  4 / 256; 
    ret += tex.Sample(smp, uv + float2(-1 * d.x, 1 * d.y)) * 16 / 256; 
    ret += tex.Sample(smp, uv + float2( 0 * d.x, 1 * d.y)) * 24 / 256; 
    ret += tex.Sample(smp, uv + float2( 1 * d.x, 1 * d.y)) * 16 / 256; 
    ret += tex.Sample(smp, uv + float2( 2 * d.x, 1 * d.y)) *  4 / 256;
    
    //5�s��
    ret += tex.Sample(smp, uv + float2(-2 * d.x, 2 * d.y)) * 1 / 256;
    ret += tex.Sample(smp, uv + float2(-1 * d.x, 2 * d.y)) * 4 / 256; 
    ret += tex.Sample(smp, uv + float2( 0 * d.x, 2 * d.y)) * 6 / 256; 
    ret += tex.Sample(smp, uv + float2( 1 * d.x, 2 * d.y)) * 4 / 256; 
    ret += tex.Sample(smp, uv + float2( 2 * d.x, 2 * d.y)) * 1 / 256;
    
    return ret;
}

//�G���{�X
float4 Emboss(float4 ret, float2 uv, float2 d)
{
    /*
        2  1  0
        1  1 -1
        0 -1 -1
    */
    ret += tex.Sample(smp, uv + float2(-d.x, -d.y)) * 2.0f;
    ret += tex.Sample(smp, uv + float2(0.0f, -d.y)) * 1.0f;
    ret += tex.Sample(smp, uv + float2(-d.x, 0.0f)) * 1.0f;
    ret += tex.Sample(smp, uv + float2(d.x, 0.0f)) * -1.0f;
    ret += tex.Sample(smp, uv + float2(0.0f, d.y)) * -1.0f;
    ret += tex.Sample(smp, uv + float2(d.x, d.y)) * -2.0f;

    return ret;
}

//�V���[�v�l�X
float4 Sharpness(float4 ret, float2 uv, float2 d)
{
    ret += tex.Sample(smp, uv + float2(0.0f, -d.y)) * -1.0f;
    ret += tex.Sample(smp, uv + float2(-d.x, 0.0f)) * -1.0f;
    ret += tex.Sample(smp, uv) * 5.0f;
    ret += tex.Sample(smp, uv + float2(d.x, 0.0f)) * -1.0f;
    ret += tex.Sample(smp, uv + float2(0.0f, d.y)) * -1.0f;

    return ret;
}

//�֊s�����o
float4 ExtractOutline(float4 ret, float2 d, float2 uv, float power = 1.0f)
{
    ret = ret * 4 -
            tex.Sample(smp, uv + float2(-d.x, 0)) - tex.Sample(smp, uv + float2(d.x, 0)) -
            tex.Sample(smp, uv + float2(0, d.y)) - tex.Sample(smp, uv + float2(0, -d.y));

    float b = dot(value, 1 - ret.rgb);
    b = pow(b, power);

    return float4(1.0f - b, 1.0f - b, 1.0f - b, 1.0f);
}

/**
* @fn float Rand(float2 co)
* @brief ���������֐�
* @param co �e�N�X�`��
* @return float �������ꂽ������Ԃ�
*/
float Rand(float2 co)
{
    return frac(sin(dot(co, float2(12.9898, 78.233))) * 43758.5453);
}

/**
* @fn float4 FrostedGlass(float2 uv, float2 wh)
* @brief �܂�K���X�����֐�
* @param uv �e�N�X�`����UV���W
* @param wh �e�N�X�`���̉����ƍ���
* @return float4 �܂�K���X�̏o�����e�N�X�`����Ԃ�
* @details ���͂̃s�N�Z�����烉���_���Ƀs�N�Z�����擾����
*/
float4 FrostedGlass(float2 uv, float2 wh)
{
    float radius = 5.0f;
    float2 _uv = float2((uv.x * wh.x) + Rand(uv) * radius * 2.0f - radius, 
                    (uv.y * wh.y) + Rand(float2(uv.y, uv.x)) * radius * 2.0f - radius);

    return float4(tex.Sample(smp, float2(_uv.x / wh.x, _uv.y / wh.y)));
}

/**
* @fn float4 Spiral(float2 uv, float2 screen, float radius = 100.0f, float strength = 10.0f)
* @brief �Q���������֐�
* @param uv �e�N�X�`����UV���W
* @param screen ��ʃT�C�Y
* @param radius �Q�̑傫��
* @param strength �Q�̋���
* @return float4 �Q�����̏o�����e�N�X�`����Ԃ�
* @details �ڍׂȐ���
*/
float4 Spiral(float2 uv, float2 screen, float radius = 100.0f, float strength = 10.0f)
{
    float2 centor = screen / 2.0f;
    float2 pos = (uv * screen) - centor;
    float len = length(pos);
    if (len >= radius)
    {
        return tex.Sample(smp, uv);
    }

    float spiral = min(max(1.0f - (len / radius), 0.0f), 1.0f) * strength;
    float2 xy = float2(pos.x * cos(spiral) - pos.y * sin(spiral), pos.x * sin(spiral) + pos.y * cos(spiral));

    return tex.Sample(smp, (xy + centor) / screen);
}

/**
* @fn float4 Fxaa(float2 uv, float2 size, float2 d, float alpha)
* @brief FXAA��K�p����֐�
* @param uv     �e�N�X�`��UV
* @param size   �e�N�X�`��������
* @param d      1�s�N�Z�����̃T�C�Y
* @param alpha  �e�N�X�`���̃A���t�@�l
* @return float4 FXAA��K�p�����摜
*/
float4 Fxaa(float2 uv, float2 size, float2 d, float alpha)
{
    FxaaTex InputFXAATex = { smp, tex };
    return float4(FxaaPixelShader(
        uv,                                     // FxaaFloat2 pos,
        FxaaFloat4(0.0f, 0.0f, size.x, size.y), // FxaaFloat4 fxaaConsolePosPos,
        InputFXAATex,                           // FxaaTex tex,
        InputFXAATex,                           // FxaaTex fxaaConsole360TexExpBiasNegOne,
        InputFXAATex,                           // FxaaTex fxaaConsole360TexExpBiasNegTwo,
        d,                                      // FxaaFloat2 fxaaQualityRcpFrame,
        FxaaFloat4(0.0f, 0.0f, 0.0f, 0.0f),     // FxaaFloat4 fxaaConsoleRcpFrameOpt,
        FxaaFloat4(0.0f, 0.0f, 0.0f, 0.0f),     // FxaaFloat4 fxaaConsoleRcpFrameOpt2,
        FxaaFloat4(0.0f, 0.0f, 0.0f, 0.0f),     // FxaaFloat4 fxaaConsole360RcpFrameOpt2,
        0.75f,                                  // FxaaFloat fxaaQualitySubpix,
        0.166f,                                 // FxaaFloat fxaaQualityEdgeThreshold,
        0.0833f,                                // FxaaFloat fxaaQualityEdgeThresholdMin,
        8.0f,                                   // FxaaFloat fxaaConsoleEdgeSharpness,
        0.125f,                                 // FxaaFloat fxaaConsoleEdgeThreshold,
        0.005f,                                 // FxaaFloat fxaaConsoleEdgeThresholdMin,
        FxaaFloat4(0.0f, 0.0f, 0.0f, 0.0f)      // FxaaFloat fxaaConsole360ConstDir,
    ).xyz, alpha);
}

//�u���[
float4 Blur(float2 uv, float diff = 0.01f)
{
    float3 color = tex.Sample(smp, uv - diff).rgb + tex.Sample(smp, uv + diff).rgb;
    return float4(color / 2, 1.0f);
}

//�s�N�Z���V�F�[�_
float4 BasicPS(Out o) : SV_TARGET
{
    //���\�[�X�J���[
    float4 texColor = tex.Sample(smp, o.uv);

    //�[�x�l
    float dep = pow(depth.Sample(smp, o.uv), 200);
    
    //���\�[�X�̃T�C�Y���擾
    float w, h;
    tex.GetDimensions(w, h);

    //1�s�N�Z�����擾
    float2 d = float2(1.0f / w, 1.0f / h);

    //�Z�s�A�J���[
    float3 sepia = float3(0.5f, 0.8f, 0.5f);
    
    //���H���Ȃ������̎w��
    if (o.uv.x + o.uv.y < 0.9f)
    {
        return Fxaa(o.uv, float2(w, h), d, texColor.a);
        //return texColor;
    }
    //return float4(dep, dep, dep, 1.0f);

    //half2 pos = o.uv * 2.0f - 1.0f;
    //half p = length(pos);
    //pos = (1.0f + 1.0f * p * p) / (1.0f + 2.0f * 1.0f) * pos;

    //return tex.Sample(smp, pos * 0.5f + 0.5f);

    //float2 tuv = float2(cos(o.uv.x) - o.uv.x, sin(o.uv.y) + o.uv.y);
    //return tex.Sample(smp, tuv);
    //return texColor;
    //return Monochrome(texColor.rgb);
    //return Emboss(texColor, o.uv, d);
    //return float4(Sepia(ExtractOutline(FrostedGlass(o.uv, float2(w, h)), d, o.uv, 6.0f).rgb, sepia), texColor.a);
    //return Sepia(texColor.rgb, sepia);
    //return Posterization(texColor.rgb);
    //return AveragingFilter(texColor, d, o.uv);
    //return GaussianFilter(texColor, d, o.uv);
    //return ExtractOutline(texColor, d, o.uv, 6.0f);
    return FrostedGlass(o.uv, float2(w, h));
    //return Spiral(o.uv, float2(w, h), 200.0f, 8.0f);
    return Fxaa(o.uv, float2(w, h), d, texColor.a);
    //return Blur(o.uv, 0.002f);
}
