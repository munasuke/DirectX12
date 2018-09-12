//テクスチャの0番レジスタとサンプラの0番レジスタを設定
Texture2D<float4> tex:register(t0);
SamplerState smp:register(s0);

cbuffer wvp : register(b0)
{
	float4x4 world;			//ワールド行列
	float4x4 view;			//ビュー行列
	float4x4 projection;	//プロジェクション行列
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
	pos = mul(world, pos);
	pos = mul(view, pos);
	pos = mul(projection, pos);

	Out o;
	o.svpos = pos;
	o.pos	= pos;
	o.uv	= uv;
	return o;
}

//PixelShader
float4 BasicPS(Out o) : SV_TARGET
{
	return float4(tex.Sample(smp, o.uv).abg, 1);
}
