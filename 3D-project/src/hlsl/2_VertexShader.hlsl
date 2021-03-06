cbuffer MatrixBuffer : register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

struct VertexInput
{
	float3 Position : SV_POSITION;
};

struct PixelInput
{
	float4 Position : SV_Position;
	matrix proj : PROJ;
};

PixelInput VS_main( VertexInput input ) 
{
	PixelInput output;
	output.Position = float4(input.Position, 1.0);
	output.proj = projectionMatrix;
	return output;
}