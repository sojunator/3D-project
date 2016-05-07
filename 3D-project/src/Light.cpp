#include "inc\Light.h"

Light::Light(DirectX::XMFLOAT3 lightDir, DirectX::XMFLOAT3 lightPos, DirectX::XMFLOAT4 lightColour, float ambientStrenght, DirectX::XMFLOAT3 cameraPos, ID3D11Device* device)
{
	m_constantBuffer = 0;
	m_lightPos = lightPos;
	m_lightColour = lightColour;
	m_lightDir = lightDir;
	m_cameraPos = DirectX::XMFLOAT4(cameraPos.x, cameraPos.y, cameraPos.z, 1.0f);
	m_ambientStrenght = ambientStrenght;

	CreateViewLight();
	CreateProjection();

	this->device = device;
}

void Light::CreateConstantBuffer()
{
	Shutdown();
	LightInformation cbData;
	cbData.m_lightPos = m_lightPos;
	cbData.m_lightColour = m_lightColour;
	cbData.m_ambientStrenght = m_ambientStrenght;
	cbData.m_cameraPos = m_cameraPos;

	cbData.m_lightDir = m_lightDir;
	cbData.m_lightView = DirectX::XMMatrixTranspose(m_lightView); // dis dont make any sense willie
	cbData.m_projection = m_projection;

	D3D11_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = sizeof(cbData);
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

void Light::updateLight(DirectX::XMFLOAT3 lightPos, DirectX::XMFLOAT4 lightColour, DirectX::XMFLOAT3 cameraPos, bool onoff)
{
	m_lightPos = lightPos;
	m_lightColour = lightColour;
	m_cameraPos = DirectX::XMFLOAT4(cameraPos.x, cameraPos.y, cameraPos.z, 1.0f);

	if (!onoff)
		m_lightColour = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);

}

void Light::CreateViewLight()
{
	DirectX::XMVECTOR up = DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(0.f, 1.0f, 0.0f));
	DirectX::XMVECTOR position = DirectX::XMLoadFloat3(&m_lightPos);
	DirectX::XMVECTOR lookAt = DirectX::XMLoadFloat3(&m_lightDir);
	m_lightView = DirectX::XMMatrixLookAtLH(position, lookAt, up);

}

void Light::CreateProjection()
{
	float fieldOfView = 3.141592654f / 4.0f;
	float screenAspect = (float)W_WITDH / (float)W_HEIGHT;
	m_projection = DirectX::XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, 0.5f, 2000.0f);
	m_projection = DirectX::XMMatrixTranspose(m_projection);
}

Light::Light()
{

}

void Light::Shutdown()
{
	if (m_constantBuffer != nullptr)
	{
		m_constantBuffer->Release();
		m_constantBuffer = 0;
	}
}