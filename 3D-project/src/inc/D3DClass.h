#ifndef D3DClASS_H
#define D3DCLASS_H

#include "defines.h"


class D3DClass
{
public:
	D3DClass(HWND handle);
	bool Intialize();
	void Clean3D(); // release all comobjects
	void InitScene(float r, float g, float b, float a);
	void PresentScene();
	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();
private:
	~D3DClass();
	HWND m_handle;
	IDXGISwapChain* m_swapChain = 0;
	ID3D11Device* m_Device = 0;
	ID3D11DeviceContext* m_Devcon = 0;
	ID3D11RenderTargetView* m_renderTargetView = 0;
	ID3D11Texture2D* m_depthStencilBuffer = 0;
	ID3D11DepthStencilView* m_depthStencilView = 0;
	ID3D11DepthStencilState* m_depthStencilState = 0;
	ID3D11RasterizerState* m_rasterState = 0;
	DirectX::XMMATRIX m_projectionMatrix;
	DirectX::XMMATRIX m_worldMatrix;
	DirectX::XMMATRIX m_orthoMatrix;

};

#endif