Texture2D NormalTexture		: register(t0);
Texture2D DiffuseTexture	: register(t1);
Texture2D SpecularTexture	: register(t2);
Texture2D PositionTexture	: register(t3);
Texture2D depthTexture		: register(t4);
Texture2D lightDepthTexture	: register(t5);

SamplerState SamplerClamp : register(s0);




cbuffer LightInfo : register(b0)
{
	matrix m_lightView;
	matrix m_projection;
	float4 m_lightColour;
	float4 m_cameraPos;
	float3 m_lightPos;
	float m_ambientStrenght;
	float3 m_lightDir;
}

struct PixelOut
{
	float4 color;
};

struct PixelInput
{
	float4 Position : SV_Position;
	matrix proj : PROJ;
};


float4 PS_main(PixelInput input) : SV_Target0
{
	PixelOut output;

	float4 tex = DiffuseTexture.Load(float3(input.Position.xy, 0), 0);
	float3 normal = NormalTexture.Load(float3(input.Position.xy, 0), 0).xyz;
	float4 specular = SpecularTexture.Load(float3(input.Position.xy, 0), 0);
	float4 position = PositionTexture.Load(float3(input.Position.xy, 0), 0);
	float4 lightDepth = lightDepthTexture.Load(float3(input.Position.xy, 0), 0);
	float4 depth = depthTexture.Load(float3(input.Position.xy, 0), 0);


	float bias = 0.0015f;

	float4 ambient = m_ambientStrenght * m_lightColour;

	matrix ligthViewProj = mul(m_lightView, m_projection);


	float4 lightViewPosition = mul(position, ligthViewProj);

	float2 projectTexCoord; 
	float depthTest;

	projectTexCoord.x = lightViewPosition.x / lightViewPosition.w;
	projectTexCoord.y = -lightViewPosition.y / lightViewPosition.w;
	depthTest = lightViewPosition.z / lightViewPosition.w;

	projectTexCoord.x = (projectTexCoord.x * 0.5) + 0.5f;
	projectTexCoord.y = (projectTexCoord.y * 0.5) + 0.5f;


	if ((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
	{
		float depthValue = depthTest;

		float lightDepthValue = lightDepthTexture.Sample(SamplerClamp, projectTexCoord).r;
		depthValue -= bias;

		if (depthValue < lightDepthValue)
		{
			float3 lightDir = normalize(m_lightPos - position.xyz);
			float diff = specular.xyz * max(dot(normal, lightDir), 0);
			float4 diffuse = diff * m_lightColour;

			float3 viewPos = m_cameraPos.xyz;
			float SpecularStrenght = 0.5;
			float3 viewDir = normalize(viewPos - position.xyz);
			float3 reflectDir = reflect(-lightDir, normal);
			float reflection = max(dot(viewDir, reflectDir), 0.0f);
			float spec = pow(reflection, 16);
			float3 specularValue = SpecularStrenght * specular.xyz * spec * m_lightColour.xyz;

			return tex  * (ambient + diffuse) + float4(specularValue, 1.0f);
		}
	}
	return tex;
}
