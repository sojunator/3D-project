#include "inc\Light.h"
#include "inc\defines.h"

Light::Light(DirectX::XMFLOAT3 lightPos, DirectX::XMFLOAT3 lightDir, DirectX::XMFLOAT4 lightColour, float ambientStrenght, DirectX::XMFLOAT3 cameraPos, ID3D11Device* device, ID3D11DeviceContext* devcon)
{
	m_constantBuffer = 0;
	m_lightPos = lightPos;
	m_lightColour = lightColour;
	m_lightDir = lightDir;
	m_cameraPos = DirectX::XMFLOAT4(cameraPos.x, cameraPos.y, cameraPos.z, 1.0f);
	m_ambientStrenght = ambientStrenght;
	m_device = device;
	m_devcon = devcon;

	GenerateViewMatrix();
	GenerateProjectionMatrix();
	CreateDepthTexture();



}

void Light::GenerateProjectionMatrix()
{
	float fieldOfView = 3.141592654f / 4.0f;
	float screenAspect = (float)W_WITDH / (float)W_HEIGHT;
	m_projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, 0.5f, 2000.0f);
}

void Light::GenerateViewMatrix()
{
	DirectX::XMVECTOR up = DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(0.f, 1.0f, 0.0f));
	DirectX::XMVECTOR position = DirectX::XMLoadFloat3(&m_lightPos);
	DirectX::XMVECTOR lookAt = DirectX::XMLoadFloat3(&m_lightDir);

	m_lightView = DirectX::XMMatrixLookAtLH(position, lookAt, up);
}

void Light::BindRtv()
{
	ID3D11ShaderResourceView* shrv = NULL;
	m_devcon->PSSetShaderResources(0, 1, &shrv);
	m_devcon->OMSetRenderTargets(1, &m_renderTargetView, NULL);
}

void Light::BindShrv()
{
	ID3D11RenderTargetView* rtv = NULL;
	m_devcon->OMSetRenderTargets(1, &rtv, NULL);
}

DirectX::XMMATRIX Light::GetViewMatrix()
{
	return m_lightView;
}

void Light::CreateDepthTexture()
{
	m_renderTargetView = 0;
	m_renderTargetTexture = 0;
	m_shaderResourceView = 0;
	this;
	D3D11_RENDER_TARGET_VIEW_DESC rtvd;
	D3D11_SHADER_RESOURCE_VIEW_DESC shrvd;
	D3D11_TEXTURE2D_DESC textureDesc;
	HRESULT hr;

	ZeroMemory(&rtvd, sizeof(rtvd));
	ZeroMemory(&shrvd, sizeof(shrvd));
	ZeroMemory(&textureDesc, sizeof(textureDesc));

	textureDesc.Width = W_WITDH;
	textureDesc.Height = W_HEIGHT;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE; // first pass its a rt, second its a shader resource
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	hr = m_device->CreateTexture2D(&textureDesc, NULL, &m_renderTargetTexture);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to create rendertarget textures, exiting", L"Fatal error", MB_OK);

	}

	rtvd.Format = textureDesc.Format;
	rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvd.Texture2D.MipSlice = 0;

	hr = m_device->CreateRenderTargetView(m_renderTargetTexture, &rtvd, &m_renderTargetView);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Faled to create render target views", L"Fatal error", MB_OK);
	}


	shrvd.Format = textureDesc.Format;
	shrvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shrvd.Texture2D.MostDetailedMip = 0;
	shrvd.Texture2D.MipLevels = 1;

	hr = m_device->CreateShaderResourceView(m_renderTargetTexture, &shrvd, &m_shaderResourceView);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to create shaderresource", L"Fatal error", MB_OK);
	}
}

Light::~Light()
{

}

void Light::CreateConstantBuffer()
{
	LightInformation cbData;
	cbData.m_lightPos = m_lightPos;
	cbData.m_lightColour = m_lightColour;
	cbData.m_ambientStrenght = m_ambientStrenght;
	cbData.m_cameraPos = m_cameraPos;
	cbData.m_lightView = m_lightView;
	cbData.m_projection = m_projectionMatrix;

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

	HRESULT hr = m_device->CreateBuffer(&cbDesc, &InitData, &m_constantBuffer);
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

	if (m_shaderResourceView)
	{
		m_shaderResourceView->Release();
		m_shaderResourceView = 0;
	}

	if (m_renderTargetView)
	{
		m_renderTargetView->Release();
		m_renderTargetView = 0;
	}

	if (m_renderTargetTexture)
	{
		m_renderTargetTexture->Release();
		m_renderTargetTexture = 0;
	}
}