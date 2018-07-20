struct Out
{
	float4 svpos : SV_POSITION;
	float4 pos : POSITION;
};

Out BasicVS(float4 pos : POSITION)
{
	Out o;
	o.svpos = pos;
	o.pos = pos;
	return o;
}

float4 BasicPS(Out o) : SV_TARGET
{
	return float4(1, 1, 1, 1);
}
