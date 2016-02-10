#include "inc\Light.h"

Light::Light(DirectX::XMFLOAT3 lightPos, DirectX::XMFLOAT4 lightColour, float ambientStrenght, ID3D11Device* device)
{
	m_lightPos = lightPos;
	m_lightColour = lightColour;
	m_ambientStrenght = ambientStrenght;
	this->device = device;
}

void Light::CreateConstantBuffer()
{
	LightInformation cbData;
	cbData.m_lightPos = m_lightPos;
	cbData.m_lightColour = m_lightColour;
	cbData.m_ambientStrenght = m_ambientStrenght;

	D3D11_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = sizeof(LightInformation);
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = &cbData;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	HRESULT hr = device->CreateBuffer(&cbDesc, &InitData, &m_constantBuffer);
	if (FAILED(hr))
		MessageBox(NULL, L"Failed to create constant buffer for lights", L"Fatal error", MB_OK);

}

Light::Light()
{

}

void Light::Shutdown()
{
	if (m_constantBuffer)
	{
		m_constantBuffer->Release();
		m_constantBuffer = 0;
	}
}