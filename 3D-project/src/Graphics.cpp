#include "inc\Graphic.h"

Graphics::Graphics(HWND handle)
{
	m_DirectX = new D3DClass(handle);
	m_DirectX->Intialize();
	m_DirectX->InitScene(1.0f, 0.0f, 0.0f, 1.0f);
	m_DirectX->PresentScene();
}