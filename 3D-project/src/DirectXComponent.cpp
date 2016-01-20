#include "inc\DirectXComponent.h"

DirectxComponent::DirectxComponent(HWND hWnd)
{
	DXGI_SWAP_CHAIN_DESC scd;
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

	scd.BufferCount = 1;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.OutputWindow = hWnd;
	scd.SampleDesc.Count = 4;
	scd.Windowed = TRUE;

	D3D11CreateDeviceAndSwapChain(NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		D3D11_CREATE_DEVICE_DEBUG,
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		&scd,
		&gSwapchain,
		&gDev,
		NULL,
		&gDevCon

		);

	ID3D11Texture2D* pBackBuffer;
	gSwapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

	gDev->CreateRenderTargetView(pBackBuffer, NULL, &backBuffer);
	pBackBuffer->Release();

	gDevCon->OMSetRenderTargets(1, &backBuffer, NULL);

	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.Height = W_HEIGHT;
	viewport.Width = W_WITDH;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MaxDepth = 1;
	viewport.MinDepth = 0;

	gDevCon->RSSetViewports(1, &viewport);

}

void DirectxComponent::SwapBackBuffer()
{
	gSwapchain->Present(0, 0);
}

DirectxComponent::~DirectxComponent()
{
	gSwapchain->Release();
	gDev->Release();
	gDevCon->Release();
}