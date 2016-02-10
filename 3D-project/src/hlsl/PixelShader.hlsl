cbuffer MaterialInfo
{
	float4 Ka;
	float4 Ks;
	float4 Kd;
};

struct PixelInput
{
	float4 position : SV_POSITION;
	float4 normal : NORMAL;
	float2 tex : TEXCOORD0;
};

Texture2D Texture;
SamplerState ss;

float4 PS_main(PixelInput input) : SV_TARGET
{
	return Texture.Sample(ss, input.tex);
}