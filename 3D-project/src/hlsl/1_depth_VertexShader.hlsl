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

struct PixelInput
{
	float4 PositionNSC : SV_POSITION;
	float4 PositionCS : POSITIONCS;
};

PixelInput VS_main( VertexInput input )
{
	PixelInput output;
	output.PositionNSC = mul(input.Position, worldMatrix);
	output.PositionNSC = mul(output.PositionNSC, viewMatrix);
	output.PositionNSC = mul(output.PositionNSC, projectionMatrix);

	output.PositionCS = output.PositionNSC;

	return output;
}