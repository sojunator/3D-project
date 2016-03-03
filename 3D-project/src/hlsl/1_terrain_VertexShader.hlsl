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
	float4 position;
	float4 color;
};

PixelInput VS_main(VertexInput input)
{
	PixelInput output;
	input.w = 1.0f; // Its a position, hence last element equals 1

	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	output.color = input.color;

	return output;
}