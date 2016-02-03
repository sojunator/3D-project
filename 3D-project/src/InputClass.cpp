#include "inc\InputClass.h"
#include "inc\defines.h"

InputClass::InputClass()
{
	ShowCursor(false);
	// All keys needs to be set too false
	for (int i = 0; i < 256; i++)
		m_keys[i] = false;

	oldMousePos.x = 400;
	oldMousePos.y = 300;
}

POINT InputClass::mousePos(HWND handle)
{
	POINT currentPos, offset;
	GetCursorPos(&currentPos);
	ScreenToClient(handle, &currentPos);

	offset.x = oldMousePos.x - currentPos.x;
	offset.y = oldMousePos.y - currentPos.y;

	ClientToScreen(handle, &oldMousePos);
	SetCursorPos(oldMousePos.x, oldMousePos.y);

	return offset;
}

LRESULT CALLBACK InputClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch (umsg)
	{
		case WM_KEYDOWN:
			if (wparam == 0x57)
				keyDown(0x57);
			if (wparam == 0x53)
				keyDown(0x53);
			if (wparam == 0x41)
				keyDown(0x41);
			if (wparam == 0x44)
				keyDown(0x44);
			if (wparam == VK_ESCAPE)
				PostQuitMessage(0);
			break;
		case WM_KEYUP:
			if (wparam == 0x57)
				keyUp(0x57);
			if (wparam == 0x53)
				keyUp(0x53);
			if (wparam == 0x41)
				keyUp(0x41);
			if (wparam == 0x44)
				keyUp(0x44);
			if (wparam == VK_ESCAPE)
				PostQuitMessage(0);

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

void InputClass::keyUp(unsigned int key)
{
	m_keys[key] = false;
}

bool InputClass::IsKeyPressed(unsigned int key)
{
	return m_keys[key];
}