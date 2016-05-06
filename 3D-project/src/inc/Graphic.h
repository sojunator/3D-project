#ifndef GRAPHICS_H
#define GRAPHICS_H

// Vs header files
#include <windows.h>
#include <vector>

// our header files
#include "defines.h"
#include "D3DClass.h"
#include "Model.h"
#include "CameraClass.h"
#include "ShaderClass.h"
#include "DeferredShader.h"
#include "Light.h"
#include "ComputeShader.h"
#include "TerrainClass.h"

#define AMOUNT_OF_LIGHTS 1

class Graphics
{
public:
	Graphics(HWND wndHandle);
	bool Render(float dt, bool* keys, POINT mousePos);
	bool Update(float dt);
	void Shutdown();

private:
	std::vector<Light> m_lights;
	float m_nrOfLights;
	D3DClass* m_DirectX;
	~Graphics();
	CameraClass* m_Camera;
	Model* m_Model;
	TerrainClass* m_map;
	ShaderClass* m_TerrainShader;
	ShaderClass* m_Shader;
	ShaderClass* m_depthShader;
	DeferredShader* m_ShaderLight;
	ComputeShader* m_GuassianShader;
	ComputeShader* m_passThrough;

};

#endif
