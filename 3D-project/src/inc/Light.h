#ifndef LIGHT_H
#define LIGHT_H

#include "defines.h"

class Light
{
public:
	Light(DirectX::XMFLOAT3 lightDir, DirectX::XMFLOAT3 lightPos, DirectX::XMFLOAT4 lightColour, float ambientStrenght, DirectX::XMFLOAT3 cameraPos, ID3D11Device* device);
	Light();
	void CreateConstantBuffer();
	void Shutdown();
	void updateLight(DirectX::XMFLOAT3 lightPos, DirectX::XMFLOAT4 lightColour, DirectX::XMFLOAT3 cameraPos, bool onoff);
	ID3D11Buffer* GetConstantBuffer()
	{
		return m_constantBuffer;
	}

	DirectX::XMMATRIX GetLightView()
	{
		return m_lightView;
	}

	DirectX::XMMATRIX GetProjection()
	{
		return m_projection;
	}

private:
	struct LightInformation
	{
		DirectX::XMMATRIX m_lightView;
		DirectX::XMMATRIX m_projection;
		DirectX::XMFLOAT4 m_lightColour;
		DirectX::XMFLOAT4 m_cameraPos;
		DirectX::XMFLOAT3 m_lightPos;
		float m_ambientStrenght;
		DirectX::XMFLOAT3 m_lightDir;
	};
	ID3D11Device* device;
	ID3D11Buffer* m_constantBuffer;
	DirectX::XMFLOAT3 m_lightPos;
	DirectX::XMFLOAT4 m_lightColour;
	DirectX::XMFLOAT3 m_lightDir;
	DirectX::XMFLOAT4 m_cameraPos;
	DirectX::XMMATRIX m_lightView;
	DirectX::XMMATRIX m_projection;
	float m_ambientStrenght;

	void CreateViewLight();
	void CreateProjection();

};

#endif