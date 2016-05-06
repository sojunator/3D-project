struct PixelInput
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD;
	float3 normal : NORMALWS;
	float4 positionWS : POSITIONWS;
	float3 tangent : TANGETWS;
	float3 binormal : BINORMALWS;
};


struct PixelOut
{
	float4 normal : SV_Target0;
	float4 diffuse : SV_Target1;
	float4 specular : SV_Target2;
	float4 position : SV_Target3;
};

Texture2D Texture : register(t0);
Texture2D NormalMap : register(t1);
SamplerState ss : register(s0);

PixelOut PS_main(PixelInput input) : SV_TARGET
{
	PixelOut output;

	float4 bumpMap = NormalMap.Sample(ss, input.tex);
	float4 bumpNormal = normalize(float4(bumpMap.x * input.tangent + bumpMap.y * input.binormal + bumpMap.z*input.normal, 0.0f));


	output.position = input.positionWS;
	output.diffuse = Texture.Sample(ss, input.tex);
	output.normal = bumpNormal;
	output.specular = float4(0.3f, 0.3f, 0.3f, 0.0f);

	return output;
}