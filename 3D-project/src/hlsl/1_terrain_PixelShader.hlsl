struct PixelInput
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

struct PixelOut
{
	float4 normal : SV_Target0;
	float4 diffuse : SV_Target1;
	float4 specular : SV_Target2;
	float4 position : SV_Target3;
};

PixelOut PS_main(PixelInput input) : SV_TARGET
{
	PixelOut output;
	output.position = input.position;
	output.diffuse = input.color;
	output.normal = float4(0.0f, 1.0f, 0.0f, 0.0f);
	output.specular = input.position;
	return output;
}