cbuffer MatrixBuffer : register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

struct VertexInput
{
	float4 position : SV_POSITION;
	float4 normal : NORMAL;
	float2 tex : TEXCOORD0;
};

struct PixelInput
{
	float4 position : SV_POSITION;
	float4 normal : NORMAL;
	float2 tex : TEXCOORD0;
	float4 worldPos : POSITION;
};

PixelInput VS_main( VertexInput input ) 
{
	PixelInput output;

	input.position.w = 1.0f; // Needs to be one for mult-purposes;
	output.worldPos = output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	input.normal.w = 0.0f;

	output.normal = mul(input.normal, worldMatrix);
	//output.normal = mul(output.normal, viewMatrix);
	//output.normal = mul(output.normal, projectionMatrix);

	output.tex = input.tex;



	return output;
}