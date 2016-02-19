struct GeometryInput
{
	float4 PositionCS : SV_Position;
	float2 Tex : TEXCOORD0;
	float3 NormalWS : NORMALWS;
	float3 PositionWS : POSITIONWS;
	float3 PositionVS : POSITIONVS;
	float3 NormalVS : NORMALVS;
};

struct PixelInput
{
	float4 PositionCS : SV_Position;
	float2 Tex : TEXCOORD0;
	float3 NormalWS : NORMALWS;
	float3 PositionWS : POSITIONWS;
};

[maxvertexcount(3)]
void GS_main(triangle GeometryInput input[3], inout TriangleStream< PixelInput > output)
{
	if (dot(input[0].PositionVS, input[0].NormalVS) < 0)
	{
		for (uint i = 0; i < 3; i++)
		{
			PixelInput element;
			element.PositionCS = input[i].PositionCS;
			element.Tex = input[i].Tex;
			element.PositionWS = input[i].PositionWS;
			element.NormalWS = input[i].NormalWS;
			output.Append(element);
		}
	}
}