#ifndef LIGHT_H
#define LIGHT_H

#include "defines.h"

class Light
{
public:
	Light(DirectX::XMFLOAT3 lightPos, DirectX::XMFLOAT3 lightDir, DirectX::XMFLOAT4 lightColour, float ambientStrenght, DirectX::XMFLOAT3 cameraPos, ID3D11Device* device, ID3D11DeviceContext* devcon);
	Light();
	~Light();
	void CreateConstantBuffer();
	void Shutdown();
	void BindRtv();
	void BindShrv();
	ID3D11ShaderResourceView* getShaderResource() { return m_shaderResourceView; };
	void updateLight(DirectX::XMFLOAT3 lightPos, DirectX::XMFLOAT4 lightColour, DirectX::XMFLOAT3 cameraPos, bool onoff);
	DirectX::XMMATRIX GetViewMatrix();
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
		DirectX::XMFLOAT4 m_lightDirection;
		DirectX::XMMATRIX m_lightView;
		DirectX::XMMATRIX m_projection;
	};

	void GenerateViewMatrix();
	void CreateDepthTexture();
	void GenerateProjectionMatrix();

	ID3D11Device* m_device;
	ID3D11DeviceContext* m_devcon;
	ID3D11Buffer* m_constantBuffer;
	DirectX::XMFLOAT3 m_lightDir;
	DirectX::XMFLOAT3 m_lightPos;
	DirectX::XMFLOAT4 m_lightColour;
	DirectX::XMFLOAT4 m_cameraPos;
	DirectX::XMMATRIX m_lightView;
	DirectX::XMMATRIX m_projectionMatrix;
	ID3D11Texture2D* m_renderTargetTexture;
	ID3D11RenderTargetView* m_renderTargetView;
	ID3D11ShaderResourceView* m_shaderResourceView;
	float m_ambientStrenght;

};

#endif