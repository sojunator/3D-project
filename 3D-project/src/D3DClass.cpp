#include "inc\D3DClass.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

D3DClass::D3DClass(HWND handle)
{
	m_handle = handle;
}

bool D3DClass::Intialize()
{
	HRESULT hr;
	IDXGIFactory* factory;
	IDXGIAdapter* adapter;
	IDXGIOutput* adapterOutput;
	DXGI_MODE_DESC* displayModeList;
	DXGI_ADAPTER_DESC adapterDesc;
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	unsigned long long stringLength;
	unsigned int numModes, i, numerator, denominator;

	// Create a factory
	hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(hr))
		MessageBox(NULL, L"Failed to create factory", L"Build more pilons", MB_OK);

	// fetch information about primary output, monitor
	hr = factory->EnumAdapters(0, &adapter);
	if (FAILED(hr))
		MessageBox(NULL, L"Factor failed to create adapter", L"Whops", MB_OK);

	hr = adapter->EnumOutputs(0, &adapterOutput);
	if (FAILED(hr))
		MessageBox(NULL, L"Failed to enumarate primary output", L"Nope", MB_OK);

	hr = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if (FAILED(hr))
		MessageBox(NULL, L"Failed to fetch numbers of modes", L"insert text", MB_OK);

	// Fetch all possible monitor and video card dispaly modes and place it in a list
	displayModeList = new DXGI_MODE_DESC[numModes];
	for (int i = 0; i < numModes; i++)
	{
		if (displayModeList[i].Width == (unsigned int)W_WITDH)
		{
			if (displayModeList[i].Height == (unsigned int)W_HEIGHT)
			{
				// Copy avaiable refresh-rates
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}


	hr = adapter->GetDesc(&adapterDesc);
	if (FAILED(hr))
		MessageBox(NULL, L"Failed to fetch gfx desc", L"gfx adapter errorror", MB_OK);

	m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	// Convert the array to a string, but it doesnt work, so fuck it for now
	/*wcstombs_s(&stringLength, m_videoCardDesc, 128, adapterDesc.Description, 128);*/


	// We dont need the displaymodes anymore nor the adapter, so release
	delete[] displayModeList;
	displayModeList = 0;

	adapterOutput->Release();
	adapterOutput = 0;

	adapter->Release();
	adapter = 0;


	// creating our swap chain and devices
	DXGI_SWAP_CHAIN_DESC scd;
	ZeroMemory(&scd, sizeof(scd));

	scd.BufferCount = 1;
	scd.BufferDesc.Height = (float)W_HEIGHT;
	scd.BufferDesc.Width = (float)W_WITDH;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.OutputWindow = m_handle;
	scd.SampleDesc.Count = 1; // AA times 4
	scd.SampleDesc.Quality = 0;
	scd.Windowed = TRUE;
	scd.BufferDesc.RefreshRate.Numerator = 0; // change 0 to numerator for vsync
	scd.BufferDesc.RefreshRate.Denominator = 1; // change 1 to denominator for vynsc

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


	// z-buffer setup
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
	depthBufferDesc.Width = (float)W_WITDH;
	depthBufferDesc.Height = (float)W_HEIGHT;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;


	// create the texture for the z-buffer
	hr = m_Device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
	if (FAILED(hr))
		MessageBox(m_handle, L"Failed to create texture for z-buffer", L"Z-buffer failure", MB_OK);

	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing.
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing.
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	hr = m_Device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
	if (FAILED(hr))
		MessageBox(m_handle, L"Failed to create z-buffer stencilstate", L"Z-buffer failure", MB_OK);

	// set state
	m_Devcon->OMSetDepthStencilState(m_depthStencilState, 1);

	// configure stencil
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	//Create the stencil
	hr = m_Device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
	if (FAILED(hr))
		MessageBox(m_handle, L"Failed to create depth stencil view", L"Error z-buffer", MB_OK);

	m_Devcon->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView); // Disabled z buffer for now m_depthStencilView

	 //Configure rasterizer
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
	viewport.Width = (float)W_WITDH;
	viewport.Height = (float)W_HEIGHT;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	// create viewport
	m_Devcon->RSSetViewports(1, &viewport);


	float fieldOfView = 3.141592654f / 4.0f;
	float screenAspect = (float)W_WITDH / (float)W_HEIGHT;
	m_projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, 0.5f, 20.0f);
	m_worldMatrix = DirectX::XMMatrixIdentity();
	m_orthoMatrix = DirectX::XMMatrixOrthographicLH(W_WITDH, W_HEIGHT, 0.5f, 20.0f);
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

D3DClass::~D3DClass()
{

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
	 m_Devcon->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
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

void D3DClass::GetProjectionMatrix(DirectX::XMMATRIX& projectionMatrix)
{
	projectionMatrix = m_projectionMatrix;
}

void  D3DClass::GetWorldMatrix(DirectX::XMMATRIX& worldMatrix)
{
	worldMatrix = m_worldMatrix;
}

void  D3DClass::GetOrthoMatrix(DirectX::XMMATRIX& orthoMatrix)
{
	orthoMatrix = m_orthoMatrix;
}