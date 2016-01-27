#include "inc\System.h"
#include "inc\Graphic.h"
#include "inc\InputClass.h"

System::System(HWND handle, InputClass* inputHandle)
{
	m_InputHandler = inputHandle;
	m_InputHandler->SetHandle(handle);
	m_Graphics = new Graphics(handle);
	dt = 0.0f;
}

bool System::Render()
{

	m_Graphics->Render(dt, wasd, m_InputHandler->DeltaPosMouse(wndHandle));
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
	wasd[0] = m_InputHandler->IsKeyPressed(0x57);
	wasd[1] = m_InputHandler->IsKeyPressed(0x41);
	wasd[2] = m_InputHandler->IsKeyPressed(0x53);
	wasd[3] = m_InputHandler->IsKeyPressed(0x44);
}

void System::DeltaTime()
{
	this->dt += 0.001f;
}