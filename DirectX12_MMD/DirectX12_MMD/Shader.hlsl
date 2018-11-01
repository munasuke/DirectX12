//�e�N�X�`��
Texture2D<float4> tex   : register(t0); //�ʏ�e�N�X�`��
Texture2D<float4> sph : register(t1); //��Z�e�N�X�`��
Texture2D<float4> spa : register(t2); //���Z�e�N�X�`��
//Texture2D<float4> toon  : register(t3); //�g�D�[���e�N�X�`��

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
    float3 normal   : NORMAL;
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
    //matrix m = boneMatrix[boneno.x] * w + boneMatrix[boneno.y] * (1 - w);
    //pos = mul(m, pos);
    pos = mul(mul(vp, world), pos);

    o.svpos = pos;
    o.pos = pos;
    o.normal = mul(world, normal);
    o.uv = uv;
    o.weight = float2(w, 1 - w);
    return o;
}

//PixelShader
float4 BasicPS(Out o) : SV_TARGET
{
	//���_
    float3 eye = (0.0f, 10.0f, -15.0f);
    //����
    float3 ray = normalize(eye - o.pos.xyz);
	//���C�g
    float3 light = normalize(float3(-1, 1, -1));
    //���˃x�N�g��
    float3 mirror = reflect(-light, o.normal);

    float spec = saturate(dot(mirror, ray));

    spec = pow(spec, specular.a);

    float brightness = saturate(dot(light, o.normal.xyz)); //rcp : ���K�������o�[�g

    

    float3 color = tex.Sample(smp, o.uv).rgb * saturate(diffuse.rgb * brightness + specular.rgb * spec + ambient.rgb);
    return float4(pow(color.r, 2.2f), pow(color.g, 2.2f), pow(color.b, 2.2f), diffuse.a);
    return float4(color, diffuse.a);
}
