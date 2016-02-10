#ifndef LIGHT_H
#define LIGHT_H

#include "defines.h"

class Light
{
public:
	Light(DirectX::XMFLOAT3 lightPos, DirectX::XMFLOAT4 lightColour, float ambientStrenght, DirectX::XMFLOAT3 cameraPos, ID3D11Device* device);
	Light();
	void CreateConstantBuffer();
	void Shutdown();
	void updateLight(DirectX::XMFLOAT3 lightPos, DirectX::XMFLOAT4 lightColour, DirectX::XMFLOAT3 cameraPos, bool onoff);
	ID3D11Buffer* GetConstantBuffer()
	{
		return m_constantBuffer;
	}
private:
	struct LightInformation
	{
		DirectX::XMFLOAT3 m_lightPos;
		float m_ambientStrenght;
		DirectX::XMFLOAT4 m_lightColour;
		DirectX::XMFLOAT4 m_cameraPos;
	};

	ID3D11Device* device;
	ID3D11Buffer* m_constantBuffer;
	DirectX::XMFLOAT3 m_lightPos;
	DirectX::XMFLOAT4 m_lightColour;
	DirectX::XMFLOAT4 m_cameraPos;
	float m_ambientStrenght;

};

#endif