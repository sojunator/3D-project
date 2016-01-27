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
private:
	void checkKeyStrokes();
	bool wasd[4];
	~System();
	float dt;
	HWND wndHandle;
	HINSTANCE m_hinstance;
	Graphics* m_Graphics = 0;
	InputClass* m_InputHandler = 0;

};

#endif
