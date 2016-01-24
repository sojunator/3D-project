#include "inc\System.h"
#include "inc\Graphic.h"

System::System(HWND handle)
{
	m_Graphics = new Graphics(handle);
}