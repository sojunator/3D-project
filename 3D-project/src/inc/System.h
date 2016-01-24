#ifndef SYSTEM_H
#define SYSTEM_H

#include <Windows.h>

#include "Graphic.h"

class System
{
public:
	System(HWND wndHandle);
private:
	~System();

	HWND wndHandle;
	HINSTANCE m_hinstance;
	Graphics* m_Graphics = 0;

};

#endif
