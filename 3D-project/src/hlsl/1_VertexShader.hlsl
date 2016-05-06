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

struct GeometryInput
{
	float4 PositionCS : SV_Position;
	float2 Tex : TEXCOORD0;
	float3 NormalWS : NORMALWS;
	float4 PositionWS : POSITIONWS;
	float3 PositionVS : POSITIONVS;
	float3 NormalVS : NORMALVS;
};

GeometryInput VS_main( VertexInput input ) 
{
	GeometryInput output;
	float4x4 worldView;
	float4x4 worldViewProjection;

	worldView = mul(worldMatrix, viewMatrix);
	worldViewProjection = mul(worldView, projectionMatrix);

	output.PositionCS = mul(input.Position, worldViewProjection);

	output.PositionWS = mul(input.Position, worldMatrix);

	output.PositionVS = mul(input.Position, viewMatrix);

	output.NormalVS = mul(input.Normal, viewMatrix);

	output.Tex = input.Tex;
	output.NormalWS = normalize(mul(input.Normal, (float3x3)worldMatrix));

	return output;
}