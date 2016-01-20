// Vs includes
#include <windows.h>

// Our includes
#include "inc\defines.h"
#include "inc\DirectXComponent.h"
#include "inc\VertexShaderComponent.h"

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")


HWND InitWindow(HINSTANCE hInstance);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	MSG msg = { 0 };

	HWND wndHandle = InitWindow(hInstance);

	DirectxComponent* d3dComponent = new DirectxComponent(wndHandle);

	if (wndHandle)
	{
		ShowWindow(wndHandle, nCmdShow);
		while (WM_QUIT != msg.message)
		{
			if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{

			}
		}

		DestroyWindow(wndHandle);
		free(d3dComponent);
	}

	return (int)msg.wParam;
}

HWND InitWindow(HINSTANCE hInstance)
{

	WNDCLASSEX wcex = { 0 };
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;		
	wcex.hInstance = hInstance;
	wcex.lpszClassName = L"BasicWindow";
	if (!RegisterClassEx(&wcex))
		return false;

	RECT rc = { 0, 0, W_HEIGHT, W_WITDH };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

	HWND handle = CreateWindow(
		L"BasicWindow",			
		L"BTH BasicWindow",		
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		rc.right - rc.left,		
		rc.bottom - rc.top,		
		nullptr,
		nullptr,
		hInstance,
		nullptr);

	return handle;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}