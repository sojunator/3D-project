struct VertexInput
{
	float4 position : POSITION;
	float4 color : COLOR;
};

struct PixelInput
{
	float4 position : POSITION;
	float4 color : COLOR;
};

PixelInput VS_main( VertexInput input ) 
{
	PixelInput output;
	output.position = input.position;
	output.color = input.color;

	return output;
}