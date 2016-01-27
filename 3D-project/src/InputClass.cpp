#include "inc\InputClass.h"
#include "inc\defines.h"

InputClass::InputClass()
{

	// All keys needs to be set too false
	for (int i = 0; i < 256; i++)
		m_keys[i] = false;
}

LPPOINT InputClass::DeltaPosMouse(HWND handle)
{
	POINT newPoint;
	GetCursorPos(&newPoint);
	ScreenToClient(handle, &newPoint);
	if (oldMousePos == NULL)
	{
		oldMousePos = &newPoint;
		return oldMousePos;
	}

	oldMousePos->x = newPoint.x;
	oldMousePos->y = newPoint.y;

	return oldMousePos;
}

LRESULT CALLBACK InputClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch (umsg)
	{
		case WM_KEYDOWN:
			switch (wparam)
			{
				case VK_ESCAPE:
					PostQuitMessage(0);
					return 0;
				case 0x57: // W key
					keyDown(0x57);
					return 0;
				case 0x53: // S key
					keyDown(0x53);
					return 0;
				case 0x41: // A key
					keyDown(0x41);
					return 0;
				case 0x44:
					keyDown(0x44);
					return 0;
			}
			break;
		case WM_KEYUP:
			switch (wparam)
			{
			case VK_ESCAPE:
				PostQuitMessage(0);
				return 0;
			case 0x57: // W key
				KeyUp(0x57);
				return 0;
			case 0x53: // S key
				KeyUp(0x53);
				return 0;
			case 0x41: // A key
				KeyUp(0x41);
				return 0;
			case 0x44:
				KeyUp(0x44);
				return 0;
			}

		default:
		{
		return DefWindowProc(hwnd, umsg, wparam, lparam);
		}
	}
}

InputClass::~InputClass()
{
	// Because c++ is a mopy little bitch
}

void InputClass::keyDown(unsigned int key)
{
	m_keys[key] = true;
}

void InputClass::KeyUp(unsigned int key)
{
	m_keys[key] = false;
}

bool InputClass::IsKeyPressed(unsigned int key)
{
	return m_keys[key];
}