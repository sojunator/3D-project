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
	float3 normal : NORMALWS;
	float3 positionWS : POSITIONWS;
	float3 tangent : TANGETWS;
	float3 binormal : BINORMALWS;
	float4 positionHCS : TEXTURE0;
};

struct VertexInput
{
	float3 position : SV_POSITION;
	float2 tex : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
};

PixelInput VS_main(VertexInput input)
{
	PixelInput output;
	
	float4 tempPos = float4(input.position.xyz, 1.0f);

	output.position = mul(tempPos, worldMatrix);
	output.positionWS = output.position;
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	output.positionHCS = output.position;

	output.normal = mul(input.normal, (float3x3)worldMatrix);
	output.binormal = normalize(mul(input.binormal, (float3x3)worldMatrix));
	output.tangent = normalize(mul(input.tangent, (float3x3)worldMatrix));

	output.normal = normalize(output.normal);

	output.tex = input.tex;

	return output;
}