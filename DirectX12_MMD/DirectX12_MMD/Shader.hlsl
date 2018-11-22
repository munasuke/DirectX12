//�e�N�X�`��
Texture2D<float4> tex   : register(t0); //�ʏ�e�N�X�`��
Texture2D<float4> sph   : register(t1); //��Z�e�N�X�`��
Texture2D<float4> spa   : register(t2); //���Z�e�N�X�`��
Texture2D<float4> toon  : register(t3); //�g�D�[���e�N�X�`��

SamplerState smp : register(s0);//�T���v��

//�J����
cbuffer wvp : register(b0)
{
    float4x4 world;         //���[���h�s��
    float4x4 view;          //�r���[�s��
    float4x4 projection;    //�v���W�F�N�V�����s��
    float4x4 lvp;           //���C�g�r���[�s��
};

//�}�e���A��
cbuffer material : register(b1)
{
    float4  diffuse;    //�����F
    float4  specular;   //����F
    float4  ambient;    //���F
    bool    texFlag;    //�e�N�X�`������Ȃ��t���O
};

//�{�[��
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

//���_�V�F�[�_
Out BasicVS(float4 pos : POSITION, float4 normal : NORMAL, float2 uv : TEXCOORD, min16uint2 boneno : BONENO, min16uint weight : WEIGHT)
{
    Out o;
	//���[���h�r���[�v���W�F�N�V����
    float4x4 vp = mul(projection, view);
    float4x4 wvp = mul(vp, world);

    //�E�F�C�g
    float w = weight / 100.0f;

    //�{�[���̉e���x
    matrix boneImpact = boneMatrix[boneno.x] * w + boneMatrix[boneno.y] * (1 - w);
    pos = mul(boneImpact, pos);

    o.pos       = mul(world, pos);
    o.svpos     = mul(wvp, pos);
    o.normal    = mul(world, normal);
    o.uv        = uv;
    o.weight    = float2(w, 1 - w);

    return o;
}


//�X�t�B�AUV�̍쐬
float2 CreateSphereUV(float3 ray, float3 normal)
{
    //��x�N�g��
    float3 upVec = float3(0.0f, 1.0f, 0.0f);

    //�E�x�N�g�����v�Z
    float3 rightVec = normalize(cross(upVec, ray));

    //�����x�N�g���ɑ΂��Ă̏�x�N�g�����v�Z
    upVec = normalize(cross(ray, rightVec));

    //�X�t�B�AUV�v�Z
    float2 uv = float2(dot(normal, rightVec), dot(normal, upVec));

    //-1�`1��0�`1�ɕϊ�
    uv = float2(0.5f, -0.5f) * (uv + float2(1.0f, -1.0f));

    return uv;
}

//�X�y�L��������
float3 CreateSpecular(float3 light, float3 normal, float3 rray)
{
    //���C�g���˃x�N�g��
    float3 mirror = reflect(-light, normal);

    //�X�y�L����
    float spec = saturate(dot(mirror, rray));

    //�X�y�L�����̋���
    spec = pow(spec, specular.a);

    return spec;
}

//�s�N�Z���V�F�[�_
float4 BasicPS(Out o) : SV_TARGET
{
	//���_
    const float3 eye = float3(0.0f, 15.0f, -40.0f);

    //�����x�N�g��
    float3 ray = o.pos.xyz - eye;

    //�t�����x�N�g��
    float3 rray = -ray;

	//���C�g
    float3 light = float3(-1.0f, 1.0f, -1.0f);
    
    //�~����
    const float pi = 3.14159265359f;

    //���邳
    float brightness = saturate(dot(light, o.normal)); //rcp : ���K�������o�[�g
    brightness = saturate(1 - acos(brightness) / pi);

    //�X�t�B�AUV�擾
    float2 spuv = CreateSphereUV(ray, o.normal);

    //�X�y�L�����擾
    float3 spec = CreateSpecular(light, o.normal, rray);

    //�g�D�[��
    float4 tToon = toon.Sample(smp, float2(0.0f, 1.0f - brightness));

    //�e�N�X�`���J���[�i�ʏ�*��Z+���Z�j
    float3 texColor = tex.Sample(smp, o.uv).rgb * sph.Sample(smp, spuv).rgb + spa.Sample(smp, spuv).rgb;

    //�}�e���A���J���[
    float3 color = saturate(tToon.rgb * diffuse.rgb * brightness + specular.rgb * spec + ambient.rgb);

    return float4(pow(texColor * color, 2.2f), diffuse.a);
}