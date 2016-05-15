#include "inc\D3DClass.h"
#include "inc\defines.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

void D3DClass::unBindBlendState()
{
	m_Devcon->OMSetBlendState(NULL, NULL, 0xffffffff);
}

D3DClass::D3DClass(HWND handle)
{
	m_handle = handle;
}

void D3DClass::CreateRenderTargetViews()
{
	for (int i = 0; i < BUFFER_COUNT; i++)
	{
		m_renderTargetViews[i] = nullptr;
		m_shaderResourceViews[i] = nullptr;
		m_renderTargetTextures[i] = nullptr;
	}
	D3D11_RENDER_TARGET_VIEW_DESC rtvd;
	D3D11_SHADER_RESOURCE_VIEW_DESC shrvd;
	D3D11_TEXTURE2D_DESC textureDesc;
	HRESULT hr;

	ZeroMemory(&rtvd, sizeof(rtvd));
	ZeroMemory(&shrvd, sizeof(shrvd));
	ZeroMemory(&textureDesc, sizeof(textureDesc));

	// Configure our g-buffer texture
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

	for (int i = 0; i < BUFFER_COUNT; i++)
	{
		hr = m_Device->CreateTexture2D(&textureDesc, NULL, &m_renderTargetTextures[i]);
		if (FAILED(hr))
		{
			MessageBox(NULL, L"Failed to create rendertarget textures, exiting", L"Fatal error", MB_OK);

		}
	}

	rtvd.Format = textureDesc.Format;
	rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvd.Texture2D.MipSlice = 0;

	// Create all the rt views
	for (int i = 0; i < BUFFER_COUNT; i++)
	{
	 hr = m_Device->CreateRenderTargetView(m_renderTargetTextures[i], &rtvd, &m_renderTargetViews[i]);
	 if (FAILED(hr))
	 {
		 MessageBox(NULL, L"Faled to create render target views", L"Fatal error", MB_OK);
	 }
	}

	shrvd.Format = textureDesc.Format;
	shrvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shrvd.Texture2D.MostDetailedMip = 0;
	shrvd.Texture2D.MipLevels = 1;

	// create all the shader resources views
	for (int i = 0; i < BUFFER_COUNT; i++)
	{
		hr = m_Device->CreateShaderResourceView(m_renderTargetTextures[i], &shrvd, &m_shaderResourceViews[i]);
		if (FAILED(hr))
		{
			MessageBox(NULL, L"Failed to create shaderresource", L"Fatal error", MB_OK);
		}
	}
}

void D3DClass::CreateBlendState()
{
	D3D11_BLEND_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.RenderTarget[0].BlendEnable = TRUE;
	bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	bd.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	bd.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;    // addition is default for this
	bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;      // use full source alpha
	bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;    // use no dest alpha
	bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	bd.IndependentBlendEnable = FALSE;    // only use RenderTarget[0]
	bd.AlphaToCoverageEnable = TRUE;    // enable alpha-to-coverage
	m_Device->CreateBlendState(&bd, &m_blendState);

}

void D3DClass::SetBlendState()
{
	m_Devcon->OMSetBlendState(m_blendState, 0, 0xffffffff);
}

void D3DClass::SetRenderTargetViews()
{
	ID3D11ShaderResourceView* shvs[BUFFER_COUNT + 1] = { 0, 0, 0, 0, 0, 0 };
	m_Devcon->CSSetShaderResources(0, BUFFER_COUNT + 1 , shvs);
	m_Devcon->PSSetShaderResources(0, BUFFER_COUNT + 1, shvs);
	m_Devcon->OMSetRenderTargets(4, m_renderTargetViews, m_depthStencilView[0]);
}

void D3DClass::PreparePostPass()
{
	// Unbind all rendertargets
	ID3D11RenderTargetView* rtvs[4] = { 0, 0, 0, 0 };
	m_Devcon->OMSetRenderTargets(4, rtvs, NULL);

	// Bind the shader resource for the compute shader
	ID3D11ShaderResourceView* shrsv[BUFFER_COUNT] = { m_shaderResourceViews[4], 0, 0, 0};
	m_Devcon->CSSetShaderResources(0, BUFFER_COUNT, shrsv);

	// Bind the UAV for input
	UINT initialCounts = -1;
	m_Devcon->CSSetUnorderedAccessViews(0, 1, &m_backBuffer, &initialCounts);

}

void D3DClass::PrepareDepthPass()
{
	/*
	This function should unbind all current RTVs.
	The only resource bound should be the second depth buffer, as a depth
	stencil resource
	*/
	ID3D11RenderTargetView* rtvs[4] = { 0, 0, 0, 0 };
	m_Devcon->OMSetRenderTargets(4, rtvs, m_depthStencilView[1]);



}

void D3DClass::PrepareLightPass()
{
	SetBackBuffer(); // Draw into the last rendertargetview, which will be used in the post process pass
	SetShaderResourceViews(); // Make the geo-data avaiable for ps
}

void D3DClass::PrePareGeoPass()
{
	unBindBlendState();
	SetRenderTargetViews();
	InitScene(0.0f, 0.0f, 0.0f, 1.0f);
}

void D3DClass::SetBackBuffer()
{
	ID3D11RenderTargetView* rtvs[4] = { m_renderTargetViews[4], 0, 0, 0 };
	m_Devcon->OMSetRenderTargets(4, rtvs, NULL); // unbind all depth buffers
}

void D3DClass::SetShaderResourceViews()
{
	ID3D11ShaderResourceView* shrv[6] = {};
	for (int i = 0; i < 4; i++)
	{
		shrv[i] = m_shaderResourceViews[i];
	}
	shrv[4] = m_shadowResourceView[0]; // manually copy in the depth stencil texture into the the resources
	shrv[5] = m_shadowResourceView[1]; // shadow map

	m_Devcon->PSSetShaderResources(0, 6, shrv);
}

void D3DClass::DefualtState()
{
	m_Devcon->RSSetState(NULL);
}

void D3DClass::WireFrameState()
{
	m_Devcon->RSSetState(m_rasterState);
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
	scd.BufferUsage = DXGI_USAGE_UNORDERED_ACCESS;
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

	hr = m_Device->CreateUnorderedAccessView(backBufferPtr, NULL, &m_backBuffer);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to create backbuffer", L"Taking orders of a whobat?", MB_OK);
		return false;
	}
	backBufferPtr->Release();
	backBufferPtr = 0;


	// z-buffer setup
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
	depthBufferDesc.Width = (float)W_WITDH;
	depthBufferDesc.Height = (float)W_HEIGHT;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	// configure stencil
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
	depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// configure shader resource view
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	ZeroMemory(&shaderResourceViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Format = DXGI_FORMAT_R32_FLOAT;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	/*

	In short, the following block will create two textures, and give them two views, one for being bound as a 
	shader resource for sampling from, and one as a depth stencil.

	*/

	// Create the textures for the depth / shadow buffers 
	for (int i = 0; i < 2; i++)
	{
		hr = m_Device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer[i]);
		if (FAILED(hr))
			MessageBox(m_handle, L"Failed to create texture for z-buffer", L"Z-buffer failure", MB_OK);

	}

	for (int i = 0; i < 2; i++)
	{
		//Create the stencil
		hr = m_Device->CreateDepthStencilView(m_depthStencilBuffer[i], &depthStencilViewDesc, &m_depthStencilView[i]);
		if (FAILED(hr))
			MessageBox(m_handle, L"Failed to create depth stencil view", L"Error z-buffer", MB_OK);


		// Create shader resource view
		hr = m_Device->CreateShaderResourceView(m_depthStencilBuffer[i], &shaderResourceViewDesc, &m_shadowResourceView[i]);
		if (FAILED(hr))
			MessageBox(m_handle, L"Failed to create shader resource view for shadowmap", L"Error z-buffer", MB_OK);

	}


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


	 //Configure rasterizer
	D3D11_RASTERIZER_DESC rasterDesc;
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_NONE;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.FillMode = D3D11_FILL_SOLID; // WIREFRAME
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
	m_Devcon->RSSetState(NULL);

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
	m_projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, 0.5f, 2000.0f);
	m_worldMatrix = DirectX::XMMatrixIdentity();
	m_orthoMatrix = DirectX::XMMatrixOrthographicLH(W_WITDH, W_HEIGHT, 0.5f, 20.0f);


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
	for (int i = 0; i < BUFFER_COUNT; i++)
	{
		m_Devcon->ClearRenderTargetView(m_renderTargetViews[i], color);
	}
	color[0] = 0;
	color[1] = 0;
	color[2] = 0;
	color[3] = 0;

	m_Devcon->ClearUnorderedAccessViewFloat(m_backBuffer, color);

	// Clear the depth buffer
	for (int i = 0; i < 2; i++)
		 m_Devcon->ClearDepthStencilView(m_depthStencilView[i], D3D11_CLEAR_DEPTH, 1.0f, 0);
	// TODO


}

void D3DClass::ShutDown()
{
	// Before shutting down set to windowed mode or when you release the swap chain it will throw an exception.
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
		for (int i = 0; i < 2; i++)
		{
			m_depthStencilView[i]->Release();
			m_depthStencilView[i] = 0;
		}
	}

	if (m_depthStencilState)
	{
		m_depthStencilState->Release();
		m_depthStencilState = 0;
	}

	if (m_depthStencilBuffer)
	{
		for (int i = 0; i < 2; i++)
		{
			m_depthStencilBuffer[i]->Release();
			m_depthStencilBuffer[i] = 0;
			
		}
	}

	if (m_shadowResourceView)
	{
		for (int i = 0; i < 2; i++)
		{
			m_shadowResourceView[i]->Release();
			m_shadowResourceView[i] = 0;
		}
	}

	if (m_renderTargetViews)
	{
		for (int i = 0; i < BUFFER_COUNT; i++)
		{
			m_renderTargetViews[i]->Release();
			m_renderTargetViews[i] = nullptr;
		}
	}

	if (m_shaderResourceViews)
	{
		for (int i = 0; i < BUFFER_COUNT; i++)
		{
			m_shaderResourceViews[i]->Release();
			m_shaderResourceViews[i] = 0;
		}
	}

	if (m_renderTargetTextures)
	{
		for (int i = 0; i < BUFFER_COUNT; i++)
		{
			m_renderTargetTextures[i]->Release();
			m_renderTargetTextures[i] = 0;
		}
	}

	if (m_blendState)
	{
		m_blendState->Release();
		m_blendState = 0;
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

	if (m_backBuffer)
	{
		m_backBuffer->Release();
		m_backBuffer = 0;
	}
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