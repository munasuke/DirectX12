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
    float3 cnormal  : NORMAL1;
    float2 uv       : TEXCOORD;
    float2 bone     : BONENO;
    float2 weight   : WEIGHT;
};

//VertexShader
Out BasicVS(float4 pos : POSITION, float4 normal : NORMAL, float2 uv : TEXCOORD, min16uint2 boneno : BONENO, min16uint weight : WEIGHT)
{
    Out o;
	//���[���h�r���[�v���W�F�N�V����
    float4x4 vp = mul(projection, view);
    float w = weight / 100.0f;
    matrix m = boneMatrix[boneno.x] * w + boneMatrix[boneno.y] * (1 - w);
    pos = mul(m, pos);
    o.pos = mul(world, pos);
    pos = mul(mul(vp, world), pos);
    o.svpos = pos;
    o.normal = mul(world, normal);
    o.cnormal = mul(view, normal);
    o.uv = uv;
    o.weight = float2(w, 1 - w);
    return o;
}

//PixelShader
float4 BasicPS(Out o) : SV_TARGET
{
	//���_
    float3 eye = (0.0f, 15.0f, -15.0f);

    //�t�����x�N�g��
    float3 ray = normalize(eye - o.pos.xyz);

    //�����x�N�g��
    float3 rray = -ray;

    //��x�N�g��
    float3 upVec = (0.0f, 1.0f, 0.0f);

    //�E�x�N�g�����v�Z
    float3 rightVec = cross(rray, upVec);
    rightVec = normalize(rightVec);

    //�����x�N�g���ɑ΂��Ă̏�x�N�g�����v�Z
    float3 upperVec = cross(rightVec, rray);
    upperVec = normalize(upperVec);

    //�X�y�L����UV�v�Z
    float2 uv = normalize(float2(dot(o.normal, rightVec), dot(o.normal, upperVec)));

    //-1�`1��0�`1�ɕϊ�
    float2 spuv = float2(0.5f, -0.5f) * (uv + float2(1.0f, -1.0f));

	//���C�g
    float3 light = normalize(float3(-1.0f, 1.0f, -1.0f));

    //���C�g���˃x�N�g��
    float3 mirror = reflect(-light, o.normal);

    float spec = saturate(dot(mirror, ray));

    spec = pow(spec, specular.a);

    //�~����
    float pi = 3.14159265359;

    //���邳
    float brightness = saturate(dot(light, o.normal.xyz)); //rcp : ���K�������o�[�g
    brightness = saturate(1 - acos(brightness) / pi);

    //�g�D�[��
    float4 tToon = toon.Sample(smp, float2(0.0f, 1.0f - brightness));

    //�ʏ�*��Z+���Z
    float3 texColor = tex.Sample(smp, o.uv).rgb * sph.Sample(smp, o.normal.xy).rgb + spa.Sample(smp, o.normal.xy).rgb;
    float3 color = texColor * saturate(tToon.rgb * diffuse.rgb * brightness + specular.rgb * spec + ambient.rgb);
    //return float4(pow(color.r, 2.2f), pow(color.g, 2.2f), pow(color.b, 2.2f), diffuse.a);
    return float4(color, diffuse.a);
}
