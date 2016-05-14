struct GeometryInput
{
	float4 PositionCS : SV_Position;
	float2 Tex : TEXCOORD0;
	float3 NormalWS : NORMALWS;
	float4 PositionWS : POSITIONWS;
	float4 PositionVS : POSITIONVS;
};

struct PixelInput
{
	float4 PositionNDCS : SV_Position;
	float2 Tex : TEXCOORD0;
	float3 NormalWS : NORMALWS;
	float4 PositionWS : POSITIONWS;
	float4 PositionCS : POSITIONCS;
};

[maxvertexcount(3)]
void GS_main(triangle GeometryInput input[3], inout TriangleStream< PixelInput > output)
{
	float3 normal = cross(input[1].PositionVS.xyz - input[0].PositionVS.xyz, input[2].PositionVS.xyz - input[0].PositionVS.xyz);
	float direction = dot(-input[0].PositionVS, float4(normal, 0.0f));
	if (direction >= 0)
	{
		for (int i = 0; i < 3; i++)
		{
			PixelInput element;
			element.PositionNDCS = input[i].PositionCS;
			element.Tex = input[i].Tex;
			element.PositionWS = input[i].PositionWS;
			element.NormalWS = input[i].NormalWS;
			element.PositionCS = input[i].PositionCS;
			output.Append(element);
		}
	}
}