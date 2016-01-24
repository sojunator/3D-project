#include "inc\System.h"
#include "inc\Graphic.h"

System::System(HWND handle)
{
	m_Graphics = new Graphics(handle);
	dt = 0.0f;
}

bool System::Render()
{
	m_Graphics->Render(dt);
	return true;
}
bool System::Update()
{
	DeltaTime(); // Has to be here, can not go into render
	m_Graphics->Update(dt);
	return true;
}

void System::DeltaTime()
{
	this->dt += 0.001f;
}