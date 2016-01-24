#ifndef SYSTEM_H
#define SYSTEM_H

#include <Windows.h>

#include "Graphic.h"

class System
{
public:
	System(HWND wndHandle);
	bool Render();
	bool Update();
	void DeltaTime();
private:
	~System();
	float dt;
	HWND wndHandle;
	HINSTANCE m_hinstance;
	Graphics* m_Graphics = 0;

};

#endif
