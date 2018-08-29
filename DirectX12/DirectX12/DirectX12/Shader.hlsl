//�e�N�X�`����0�ԃ��W�X�^�ƃT���v����0�ԃ��W�X�^��ݒ�
Texture2D<float4> tex:register(t0);
SamplerState smp:register(s0);

struct Out
{
	float4 svpos : SV_POSITION;
	float4 pos : POSITION;
	float2 uv : TEXCOORD;
};

Out BasicVS(float4 pos : POSITION, float2 uv : TEXCOORD)
{
	Out o;
	o.svpos = pos;
	o.pos = pos;
	o.uv = uv;
	return o;
}

float4 BasicPS(Out o) : SV_TARGET
{
	return float4(tex.Sample(smp, o.uv).abg, 1);
}
