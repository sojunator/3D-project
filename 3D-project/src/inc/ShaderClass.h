#ifndef SHADERCLASS_H
#define SHADERCLASS_H

#include "defines.h"

class ShaderClass
{
public:
	ShaderClass();
	bool Initialize(ID3D11Device* device, HWND handle, WCHAR* vsFilename, WCHAR* psFilename);
	bool Render(ID3D11DeviceContext* deviceContext, int indexCount, const DirectX::XMMATRIX& world, const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection);
	void ShutDown();
	~ShaderClass();
private:
	struct MatrixBufferType
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;
	};
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11SamplerState* m_sampleState;

	bool InitializeShader(ID3D11Device* device, HWND handle, WCHAR* vsFilename, WCHAR* psFilename);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob* blob, HWND errorString, WCHAR* dno);
	bool SetShaderParameters(ID3D11DeviceContext* devcon, const DirectX::XMMATRIX& world, const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection);
	void RenderShader(ID3D11DeviceContext* devcon, int indexCount);

};

#endif
