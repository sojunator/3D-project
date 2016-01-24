#include "inc\D3DClass.h"

D3DClass::D3DClass(HWND handle)
{
	m_handle = handle;
}

bool D3DClass::Intialize()
{
	HRESULT hr;
	// creating our swap chain and devices
	DXGI_SWAP_CHAIN_DESC scd;
	ZeroMemory(&scd, sizeof(scd));

	scd.BufferCount = 1;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.OutputWindow = m_handle;
	scd.SampleDesc.Count = 4; // AA times 4
	scd.Windowed = TRUE;

	hr = D3D11CreateDeviceAndSwapChain(NULL, // adapter
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		D3D11_CREATE_DEVICE_DEBUG,
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		&scd,
		&m_swapChain,
		&m_Device,
		NULL, // feature levels
		&m_Devcon);

	if (FAILED(hr))
	{
		MessageBox(NULL, L"Swapchain failed to init", L"Yer dungoofed", MB_OK);
		return false;
	}

	// setting backbufer
	ID3D11Texture2D* backBufferPtr;
	hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to get backbuffer", L"Im a whobat beep bap", MB_OK);
		return false;
	}

	hr = m_Device->CreateRenderTargetView(backBufferPtr, NULL, &m_renderTargetView);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to create rendertargetview", L"Taking orders of whobat?", MB_OK);
		return false;
	}
	backBufferPtr->Release();
	backBufferPtr = 0; // Set it to null


	// TODO CREATE DEPTH STENCIL VIEW HERE BUT CBA AM I RITE?
	m_Devcon->OMSetRenderTargets(1, &m_renderTargetView, NULL);

	// Configure rasterizer
	D3D11_RASTERIZER_DESC rasterDesc;
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// Create rasterizer
	hr = m_Device->CreateRasterizerState(&rasterDesc, &m_rasterState);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to create rasterizerState", L"EROROROR", MB_OK);
		return false;
	}

	// set it
	m_Devcon->RSSetState(m_rasterState);

	// Configure viewport
	D3D11_VIEWPORT viewport;
	viewport.Width = W_WITDH;
	viewport.Height = W_HEIGHT;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	// create viewport
	m_Devcon->RSSetViewports(1, &viewport);


	// TODO create final matrix, take the code from assignemnt 2;
}

void D3DClass::Clean3D()
{
	if (m_swapChain)
	{
		m_swapChain->SetFullscreenState(false, NULL);
	}

	if (m_rasterState)
	{
		m_rasterState->Release();
		m_rasterState = 0;
	}

	if (m_depthStencilView)
	{
		m_depthStencilView->Release();
		m_depthStencilView = 0;
	}

	if (m_depthStencilState)
	{
		m_depthStencilState->Release();
		m_depthStencilState = 0;
	}

	if(m_depthStencilBuffer)
	{
		m_depthStencilBuffer->Release();
		m_depthStencilBuffer = 0;
	}

	if (m_renderTargetView)
	{
		m_renderTargetView->Release();
		m_renderTargetView = 0;
	}

	if (m_Devcon)
	{
		m_Devcon->Release();
		m_Devcon = 0;
	}

	if (m_Device)
	{
		m_Device->Release();
		m_Device = 0;
	}

	if (m_swapChain)
	{
		m_swapChain->Release();
		m_swapChain = 0;
	}

}

void D3DClass::InitScene(float r, float g, float b, float a)
{
	float color[4];

	color[0] = r;
	color[1] = g;
	color[2] = b;
	color[3] = a;

	// Clear the back buffer
	 m_Devcon->ClearRenderTargetView(m_renderTargetView, color);

	// Clear the depth buffer
	// TODO


}

void D3DClass::PresentScene()
{
	m_swapChain->Present(0, 0);
}

ID3D11Device* D3DClass::GetDevice()
{
	return m_Device;
}

ID3D11DeviceContext* D3DClass::GetDeviceContext()
{
	return m_Devcon;
}