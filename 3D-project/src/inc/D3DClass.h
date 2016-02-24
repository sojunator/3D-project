#ifndef D3DClASS_H
#define D3DCLASS_H

#include "defines.h"

#define BUFFER_COUNT 5

class D3DClass
{
public:
	D3DClass(HWND handle);
	bool Intialize();
	
	void PresentScene();
	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();
	void PrepareLightPass();
	void PreparePostPass();
	void PrePareGeoPass();
	void GetProjectionMatrix(DirectX::XMMATRIX& projectionMatrix);
	void GetWorldMatrix(DirectX::XMMATRIX& worldMAtrix);
	void GetOrthoMatrix(DirectX::XMMATRIX& orthoMatrix);
	void ShutDown();


	void CreateRenderTargetViews();
	void SetBlendState();
	void CreateBlendState();

	~D3DClass();
private:
	bool m_vsync_enabled;
	int m_videoCardMemory;
	char m_videoCardDesc[128];

	void SetShaderResourceViews();
	void SetBackBuffer();

	void unBindBlendState();
	void SetRenderTargetViews();
	void InitScene(float r, float g, float b, float a);

	HWND m_handle;
	IDXGISwapChain* m_swapChain = 0;
	ID3D11Device* m_Device = 0;
	ID3D11DeviceContext* m_Devcon = 0;
	ID3D11RenderTargetView* m_renderTargetViews[BUFFER_COUNT], *m_backBuffer;
	ID3D11ShaderResourceView* m_shaderResourceViews[BUFFER_COUNT];
	ID3D11Texture2D* m_renderTargetTextures[BUFFER_COUNT];
	ID3D11Texture2D* m_depthStencilBuffer = 0;
	ID3D11DepthStencilView* m_depthStencilView = 0;
	ID3D11DepthStencilState* m_depthStencilState = 0;
	ID3D11RasterizerState* m_rasterState = 0;
	ID3D11BlendState* m_blendState = 0;
	ID3D11UnorderedAccessView* m_uavPost = 0;
	DirectX::XMMATRIX m_projectionMatrix;
	DirectX::XMMATRIX m_worldMatrix;
	DirectX::XMMATRIX m_orthoMatrix;


};

#endif
