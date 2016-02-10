cbuffer MaterialInfo : register(b0)
{
	float4 Ka;
	float4 Ks;
	float4 Kd;
};

cbuffer LightInfo : register(b1)
{
	float3 m_lightPos;
	float m_ambientStrenght;
	float4 m_lightColour;
	float4 m_cameraPos;
}

struct PixelInput
{
	float4 position : SV_POSITION;
	float4 normal : NORMAL;
	float2 tex : TEXCOORD0;
	float4 worldPos : POSITION;
};

Texture2D Texture;
SamplerState ss;

float4 PS_main(PixelInput input) : SV_TARGET
{
	// Calculate ambient
	float4 ambient = m_ambientStrenght * m_lightColour;

	// Calculate diffuse
	float4 normal = normalize(input.normal);
	float3 lightDir = normalize(m_lightPos - input.worldPos.xyz);
	float diff = Kd * max(dot(normal, lightDir), 0);
	float4 diffuse = diff * m_lightColour;

	float3 viewPos = m_cameraPos.xyz;
	float SpecularStrenght = 0.5f;
	float3 viewDir = normalize(viewPos - input.worldPos.xyz);
	float3 reflectDir = reflect(-lightDir, normal);
	float reflection = max(dot(viewDir, reflectDir), 0.0f);
	float spec = pow(reflection, 32);
	float3 specular = SpecularStrenght * Ks * spec * m_lightColour.xyz;

	return Texture.Sample(ss, input.tex) * (ambient + diffuse + float4(specular, 1.0f));
}