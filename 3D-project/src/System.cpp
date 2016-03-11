#include "inc\System.h"
#include "inc\Graphic.h"
#include "inc\InputClass.h"
#include <mmsystem.h>


#pragma comment(lib, "Winmm.lib")

System::System(HWND handle, InputClass* inputHandle)
{
	m_InputHandler = inputHandle;
	m_InputHandler->SetHandle(handle);
	m_Graphics = new Graphics(handle);
	dt = 0.0f;
}

bool System::Render()
{
    m_keys = m_InputHandler->keyState();
	m_Graphics->Render(dt, m_keys, m_InputHandler->mousePos(wndHandle));
	return true;
}
bool System::Update()
{
	DeltaTime(); // Has to be here, can not go into render
	checkKeyStrokes();
	m_Graphics->Update(dt);
	return true;
}

void System::checkKeyStrokes()
{

}

void System::DeltaTime()
{
	DWORD newTime = timeGetTime();
	dt = (float)(newTime - oldTime) / 1000;
	oldTime = newTime;
}

void System::shutDown()
{
	m_Graphics->Shutdown();
}

System::~System()
{

}