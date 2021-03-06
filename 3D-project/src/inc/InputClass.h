#ifndef INPUTCLASS_H
#define INPUTCLASS_H

#include "defines.h";

class InputClass
{
public:
	InputClass();
	LRESULT CALLBACK MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam);
	bool* keyState() { return m_keys; };
	void keyUp(unsigned int key);
	void keyDown(unsigned int key);
	POINT mousePos(HWND handle);
	void SetHandle(HWND handle)
	{
		m_handle = handle;
	}
	~InputClass();
private:
	HWND m_handle;
	POINT oldMousePos;
	bool  m_keys[256];

};

#endif
