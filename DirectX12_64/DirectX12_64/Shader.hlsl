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
	float3	diffuse;	//�����F
	bool	texFlag;
	//float	alpha;
	//float	specularPower;
	//float3	specular;
	//float3	ambient;	//���F
};

struct Out
{
	float4 svpos	: SV_POSITION;
	float4 pos		: POSITION;
	float3 normal	: NORMAL;
	float2 uv		: TEXCOORD;
};

//VertexShader
Out BasicVS(float4 pos : POSITION, float4 normal : NORMAL, float2 uv : TEXCOORD)
{
	Out o;
	//���[���h�r���[�v���W�F�N�V����
	float4x4 vp = mul(projection, view);
	pos = mul(mul(vp, world), pos);

	o.svpos		= pos;
	o.pos		= pos;
	o.normal	= mul(world, normal);
	o.uv		= uv;
	return o;
}

//PixelShader
float4 BasicPS(Out o) : SV_TARGET
{
	//return float4(world[0][3], world[1][2], world[2][1], world[3][0]);
	//return float4(tex.Sample(smp, o.uv).abg, 1);

	////���_
 //   float3 eye			= (0.0f, 10.0f, -15.0f);
 //   //����
 //   float3 ray = o.pos.rgb - eye;

	//���C�g
	float3	light		= normalize(float3(-1, 1, -1));
	float	brightness	= saturate(dot(o.normal, light)) * 1.2f;
	float3	color		= texFlag ? tex.Sample(smp, o.uv).rgb : diffuse;
	return float4(color * brightness, 1);
}
