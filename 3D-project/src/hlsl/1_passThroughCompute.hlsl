Texture2D<float4> InputMap : register(t0);
RWTexture2D<float4> OutputMap : register(u0);

[numthreads(40, 20, 1)]
void CS_main(uint3 DTid : SV_DispatchThreadID)
{

	float4 Color = InputMap.Load(DTid);

	OutputMap[DTid.xy] = Color;
}