struct PixelInput
{
	float4 PositionNSC : SV_POSITION;
	float4 PositionCS : POSITIONCS;
};

float4 PS_main(PixelInput input) : SV_TARGET0
{
	float depthValue = input.PositionCS.z / input.PositionCS.w;
	float4 depth = float4(depthValue, depthValue, depthValue, 1.0f);
	return depth;
}