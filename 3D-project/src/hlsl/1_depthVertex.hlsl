cbuffer MatrixBuffer : register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

struct VertexInput
{
	float4 Position : SV_POSITION;
	float4 Normal : NORMAL;
	float2 Tex : TEXCOORD0;
};

float4 VS_main( VertexInput input ) : SV_POSITION
{
	float4 position = input.Position;
	position = mul(position, worldMatrix);
	position = mul(position, viewMatrix);
	return position = mul(position, projectionMatrix);
}