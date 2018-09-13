//�e�N�X�`����0�ԃ��W�X�^�ƃT���v����0�ԃ��W�X�^��ݒ�
Texture2D<float4> tex:register(t0);
SamplerState smp:register(s0);

cbuffer wvp : register(b0)
{
	float4x4 world;			//���[���h�s��
	float4x4 view;			//�r���[�s��
	float4x4 projection;	//�v���W�F�N�V�����s��
};

struct Out
{
	float4 svpos : SV_POSITION;
	float4 pos : POSITION;
	float2 uv : TEXCOORD;
};

//VertexShader
Out BasicVS(float4 pos : POSITION, float2 uv : TEXCOORD)
{
	//pos = mul(projection, mul(view, mul(world, pos)));
	//pos = mul(projection, mul(view, pos));
	pos.w = 1;
	//pos = mul(world, pos);
	Out o;
	o.svpos = pos;
	o.pos	= pos;
	o.uv	= uv;
	return o;
}

//PixelShader
float4 BasicPS(Out o) : SV_TARGET
{
	return float4(world[0][3], world[1][2], world[2][1], world[3][0]);
	//return float4(tex.Sample(smp, o.uv).abg, 1);
}
