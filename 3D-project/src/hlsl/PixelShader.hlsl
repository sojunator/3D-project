#define LIGHTS 1

cbuffer MaterialInfo : register(b0)
{
	float4 Ka;
	float4 Ks;
	float4 Kd;
	float4 camPos : POSITION;
	float4 LightPos;
};

//cbuffer LightInfo : register(b1)
//{
//
//};

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
	float4 diffuse;
	float4 specular;
	float4 LightToPoint = float4(normalize(LightPos.xyz - input.position.xyz), 0.0f);

	//diffuse		
	diffuse = max(0.0f, dot(LightToPoint, input.normal)) * Kd; //Ld = 1

	//specular
	float4 ViewVector = float4(normalize(camPos.xyz - input.position.xyz), 0.0f);
	float4 Reflection = 2 * dot(LightToPoint, input.normal) * input.normal - LightToPoint;

	specular = pow(dot(Reflection, ViewVector), 16) * Ks; //spec. contr. = 1

	//emissive
	//0 for now

	return (specular + diffuse) * Texture.Sample(ss, input.tex);
}