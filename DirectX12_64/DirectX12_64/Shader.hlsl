//�e�N�X�`����0�ԃ��W�X�^�ƃT���v����0�ԃ��W�X�^��ݒ�
Texture2D<float4>	tex:register(t0);
SamplerState		smp:register(s0);

cbuffer wvp : register(b0)
{
	float4x4	world;		//���[���h�s��
	float4x4	view;		//�r���[�s��
	float4x4	projection;	//�v���W�F�N�V�����s��
};

cbuffer material : register(b1)
{
	float3		diffuse;
};

struct Out
{
	float4 svpos	: SV_POSITION;
	float4 pos		: POSITION;
	float3 normal	: NORMAL;
	float2 uv		: TEXCOORD;
};

//VertexShader
Out BasicVS(float4 pos : POSITION, float3 normal : NORMAL/*, float2 uv : TEXCOORD*/)
{
	Out o;
	//���[���h�r���[�v���W�F�N�V����
	float4x4 vp = mul(projection, view);
	pos = mul(mul(vp, world), pos);

	o.svpos = pos;
	o.pos	= pos;
	o.normal = mul(world, normal);
	//o.uv	= uv;
	return o;
}

//PixelShader
float4 BasicPS(Out o) : SV_TARGET
{
	//return float4(world[0][3], world[1][2], world[2][1], world[3][0]);
	//return float4(tex.Sample(smp, o.uv).abg, 1);
	//float3 light = normalize(float3(-1, 1, -1));
	//float brightness = dot(o.normal, light);
	//return float4(brightness, brightness, brightness, 1);
	return float4(diffuse, 1);
}
