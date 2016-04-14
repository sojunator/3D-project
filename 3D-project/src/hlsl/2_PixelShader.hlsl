Texture2D NormalTexture		: register(t0);
Texture2D DiffuseTexture	: register(t1);
Texture2D SpecularTexture	: register(t2);
Texture2D PositionTexture	: register(t3);
Texture2D DepthLight		: register(t4);

SamplerState ClampSampler	: register(s0);
SamplerState WrapSampler	: register(s1);

cbuffer LightInfo : register(b0)
{
	float3 m_lightPos;
	float m_ambientStrenght;
	float4 m_lightColour;
	float4 m_cameraPos;
	matrix m_lightView;
}

struct PixelOut
{
	float4 color;
};

struct PixelInput
{
	float4 Position : SV_Position;
	matrix projectionMatrix : PROJECTIONM;
};

float4 PositionLightView(float4 position, matrix projectionMatrix)
{
	float4 returnPosition = float4(position.xyz, 1.0f);
	returnPosition = mul(returnPosition, m_lightView);
	returnPosition = mul(returnPosition, projectionMatrix);
	return returnPosition;
}

float3 PS_main(PixelInput input) : SV_Target0
{
	PixelOut output;
	float bias = 0.001f;
	
	float4 ambient = m_ambientStrenght * m_lightColour;
	float color = ambient;

	float4 tex = DiffuseTexture.Load(float3(input.Position.xy, 0), 0);
	float3 normal = NormalTexture.Load(float3(input.Position.xy, 0), 0).xyz;
	float4 specular = SpecularTexture.Load(float3(input.Position.xy, 0), 0);
	float4 position = PositionTexture.Load(float3(input.Position.xy, 0), 0);
	//float4 positionWS = PositionWSTexture.Load(float3(input.Position.xyz), 0);

	float4 lightViewPosition = PositionLightView(position, input.projectionMatrix);
	float2 projectTexCoord;

	projectTexCoord.x = lightViewPosition.x / lightViewPosition.w / 2.0f + 0.5f;
	projectTexCoord.y = lightViewPosition.y / lightViewPosition.w / 2.0f + 0.5f;

	if ((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
	{
		float depthValue = PositionTexture.Sample(ClampSampler, projectTexCoord).w;
		float lightDepthValue = DepthLight.Sample(ClampSampler, projectTexCoord).r;
		depthValue -= bias;

		if (depthValue > lightDepthValue)
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

			return tex * (ambient + diffuse) + float4(specularValue, 1.0f);
		}
	}

	return tex * ambient;
}