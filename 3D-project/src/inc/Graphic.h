#ifndef GRAPHICS_H
#define GRAPHICS_H

// Vs header files
#include <windows.h>


// our header files
#include "defines.h"
#include "D3DClass.h"

class Graphics
{
public:
	Graphics(HWND wndHandle);
	bool Render(float dt);
	bool Update(float dt);

private:
	D3DClass* m_DirectX;
	~Graphics();

};

#endif
