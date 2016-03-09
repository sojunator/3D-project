struct PixelInput
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD;
	float3 normal : NORMALWS;
	float3 positionWS : POSITIONWS;
};

struct PixelOut
{
	float4 normal : SV_Target0;
	float4 diffuse : SV_Target1;
	float4 specular : SV_Target2;
	float4 position : SV_Target3;
};

Texture2D Texture : register(t0);
SamplerState ss : register(s0);

PixelOut PS_main(PixelInput input) : SV_TARGET
{
	PixelOut output;
	output.position = float4(input.positionWS, 1.0f);
	output.diffuse = Texture.Sample(ss, input.tex);
	output.normal = normalize(float4(input.normal, 0.0f));
	output.specular = float4(0.3f, 0.3f, 0.3f, 0.0f);
	return output;
}