cbuffer MatrixBuffer : register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

struct PixelInput
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD;
	float3 normal : NORMAL;
};

struct VertexInput
{
	float3 position : SV_POSITION;
	float2 tex : TEXCOORD;
	float3 normal : NORMAL;
};

PixelInput VS_main(VertexInput input)
{
	PixelInput output;
	
	float4 tempPos = float4(input.position.xyz, 1.0f);

	output.position = mul(tempPos, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);
	output.normal = mul(input.normal, worldMatrix);

	output.tex = input.tex;

	return output;
}