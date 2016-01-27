struct PixelInput
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

float4 PS_main(PixelInput input) : SV_TARGET
{
	return input.color;
}