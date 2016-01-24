#include "inc\Graphic.h"
#include <math.h>

Graphics::Graphics(HWND handle)
{
	m_DirectX = new D3DClass(handle);
	m_DirectX->Intialize();


}

bool Graphics::Update(float dt)
{
	return true;
}

bool Graphics::Render(float dt)
{
	m_DirectX->InitScene(sinf(dt) * 0.5f, sinf(dt) * 0.3f, 0.2f, 1.0f);
	m_DirectX->PresentScene();
	return true;
}