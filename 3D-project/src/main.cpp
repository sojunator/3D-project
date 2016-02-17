// Vs includes
#include <windows.h>

// Our includes
#include "inc\defines.h"
#include "inc\System.h"
#include "inc\InputClass.h"
#undef private

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

HWND InitWindow(HINSTANCE hInstance);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Create our input handler
InputClass inputHandler;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	CoInitialize(NULL);
	MSG msg = { 0 };

	HWND wndHandle = InitWindow(hInstance);
	// Create our system class
	System* controller = new System(wndHandle, &inputHandler);


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
				controller->Update();
				controller->Render();
			}
		}

		DestroyWindow(wndHandle);
	}
	ID3D11Debug* debug;
	//(*(Graphics)((char*)controller + sizeof(HWND) + sizeof(HINSTANCE)) + sizeof(Light) + sizeof(float));
	//DXGIGetDebugInterface(__uuidof(IDXGIDebug), (void**)&debug);
	controller->shutDown();
	delete controller;
	CoUninitialize();
	//debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_DETAIL);
	//debug->Release();
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

	RECT rc = { 0, 0, W_WITDH, W_HEIGHT };
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

	return inputHandler.MessageHandler(hWnd, message, wParam, lParam);
}