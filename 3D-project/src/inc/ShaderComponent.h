#ifndef SHADERCOMPONENT
#define SHADERCOMPONENT

#include "defines.h"

class ShaderComponent
{
public:
	ShaderComponent(LPCWSTR shaderName, LPCSTR shadermain, LPCSTR shadertype);
private:
	ID3D10Blob* shader;
	ID3D10Blob* error;
	ShaderComponent();
	~ShaderComponent();
};

#endif