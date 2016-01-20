#ifndef VERTEXSHADERCOMPONENT
#define VERTEXSHADERCOMPONENT

#include "defines.h"
#include "DirectXComponent.h"

class VertexShaderComponent
{
public:
	VertexShaderComponent(LPCWSTR shaderName, LPCSTR shadermain, LPCSTR shadertype, ID3D11Device* gDev, ID3D11DeviceContext* gDevCon);
private:
	ID3D11VertexShader *pVS;
	ID3D10Blob* shader;
	ID3D10Blob* error;
	VertexShaderComponent();
	~VertexShaderComponent();
};

#endif