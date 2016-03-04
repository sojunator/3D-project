cbuffer MatrixBuffer : register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

struct PixelInput
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

struct VertexInput
{
	float3 position : SV_POSITION;
	float4 color : COLOR;
};

PixelInput VS_main(VertexInput input)
{
	PixelInput output;
	
	float4 tempPos = float4(input.position.xyz, 1.0f);

	output.position = mul(tempPos, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	output.color = input.color;

	return output;
}