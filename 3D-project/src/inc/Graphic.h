#ifndef GRAPHICS_H
#define GRAPHICS_H

// Vs header files
#include <windows.h>


// our header files
#include "defines.h"
#include "D3DClass.h"
#include "Model.h"
#include "CameraClass.h"
#include "ShaderClass.h"
#include "Light.h"

class Graphics
{
public:
	Graphics(HWND wndHandle);
	bool Render(float dt, bool wasd[4], POINT mousePos);
	bool Update(float dt);
	void Shutdown();

private:
	Light m_lights;
	float m_nrOfLights;
	D3DClass* m_DirectX;
	~Graphics();
	CameraClass* m_Camera;
	Model* m_Model;
	ShaderClass* m_Shader;
};

#endif
