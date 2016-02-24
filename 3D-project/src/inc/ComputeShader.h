#ifndef COMPUTESHADER_H
#define COMPUTESHADER_H

#include "defines.h"

class ComputeShader
{
public:
	ComputeShader();
	void Initialize(ID3D11Device* device, HWND handle, WCHAR* csFilename);
	void Render(ID3D11DeviceContext* devcon, DirectX::XMFLOAT3 groups);
	void ShutDown();
private:
	ID3D11ComputeShader* m_computeShader;
	void InitializeShader(ID3D11Device* device, HWND handle, WCHAR* csFilename);
	void RenderShader(ID3D11DeviceContext* devcon, DirectX::XMFLOAT3 groups);
};

#endif
