#ifndef SYSTEM_H
#define SYSTEM_H

#include <Windows.h>

#include "Graphic.h"
#include "InputClass.h"

class System
{
public:
	System(HWND wndHandle, InputClass* inputHandler);
	bool Render();
	bool Update();
	void DeltaTime();
	void shutDown();
	~System();
private:
	void checkKeyStrokes();
	bool* m_keys;
	float dt;
	HWND wndHandle;
	HINSTANCE m_hinstance;
	Graphics* m_Graphics = 0;
	InputClass* m_InputHandler = 0;
	DWORD oldTime = 0;
};

#endif
