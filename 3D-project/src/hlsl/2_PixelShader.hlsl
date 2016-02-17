Texture2DMS<float4, 4>  NormalTexture		: register(t0);
Texture2DMS<float4, 4>  DiffuseTexture	: register(t1);
Texture2DMS<float4, 4>  SpecularTexture	: register(t2);
Texture2DMS<float4, 4>  PositionTexture	: register(t3);

SamplerState ss : register(s0);

cbuffer LightInfo : register(b0)
{
	float3 m_lightPos;
	float m_ambientStrenght;
	float4 m_lightColour;
	float4 m_cameraPos;
}

struct PixelOut
{
	float4 color;
};

struct PixelInput
{
	float4 Position : SV_Position;
};


float4 PS_main(PixelInput input) : SV_Target0
{
	PixelOut output;

	float4 color = DiffuseTexture.Load(float3(input.Position.xy, 0), 0);
	float4 normal = NormalTexture.Load(float3(input.Position.xy, 0), 0);
	float4 specular = SpecularTexture.Load(float3(input.Position.xy, 0), 0);
	float4 position = PositionTexture.Load(float3(input.Position.xy, 0), 0);

	float4 ambient = m_ambientStrenght * m_lightColour;
	float3 lightDir = normalize(m_lightPos - position.xyz);
	float diff = specular.xyz * max(dot(normal.xyz, lightDir), 0);
	float4 diffuse = diff * m_lightColour;

	float3 viewPos = m_cameraPos.xyz;
	float SpecularStrenght = 0.5;
	float3 viewDir = normalize(viewPos - position.xyz);
	float3 reflectDir = reflect(-lightDir, normal);
	float reflection = max(dot(viewDir, reflectDir), 0.0f);
	float spec = pow(reflection, specular.w);
	float3 specularValue = SpecularStrenght * specular.xyz * spec * m_lightColour.xyz;

	return color * (ambient + diffuse + float4(specularValue, 1.0f));
}