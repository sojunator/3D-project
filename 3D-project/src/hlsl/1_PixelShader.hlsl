cbuffer MaterialInfo : register(b0)
{
	float4 Ka;
	float4 Ks;
	float4 Kd;
};

struct PixelInput
{
	float4 PositionNDCS: SV_Position;
	float2 Tex : TEXCOORD0;
	float3 NormalWS : NORMALWS;
	float3 Position : POSITIONWS;
	float4 PositionCS : POSITIONCS;
};

struct PixelOut
{
	float4 normal : SV_Target0;
	float4 diffuse : SV_Target1;
	float4 specular : SV_Target2;
	float4 position : SV_Target3;
	float4 positionWS : SV_Target4;
};

Texture2D Texture : register(t0);
SamplerState ss : register(s0);

PixelOut PS_main(PixelInput input) : SV_TARGET
{
	PixelOut output;

	float depthValue = input.PositionCS.z / input.PositionCS.w;

	output.normal = normalize(float4(input.NormalWS, 0.0));
	output.diffuse = Kd*Texture.Sample(ss, input.Tex);
	output.specular = float4(Ks.xyz, 32);
	output.position = float4(input.Position, depthValue);
	output.positionWS = float4(input.Position, 1.0f);

	return output;
}