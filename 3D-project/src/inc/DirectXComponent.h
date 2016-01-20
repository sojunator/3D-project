#ifndef DIRECTXCOMPONENT
#define DIRECTXCOMPONENT
#include "defines.h"

class DirectxComponent
{
public:
	DirectxComponent(HWND hWnd);
	void SwapBackBuffer();
	ID3D11Device* gDev;
	ID3D11DeviceContext* gDevCon;
	IDXGISwapChain* gSwapchain;
	ID3D11RenderTargetView* backBuffer;

private:
	DirectxComponent();
	~DirectxComponent();

};

#endif