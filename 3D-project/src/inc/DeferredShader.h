#ifndef DEFERREDSHADER_H
#define DEFERREDSHADER_H

#include "defines.h"

class DeferredShader
{
public:
	DeferredShader();
	void configureShader(ID3D11DeviceContext* deviceContext, const DirectX::XMMATRIX& world, const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection);
	bool Initialize(ID3D11Device* device, HWND handle, WCHAR* vsFilename, WCHAR* psFilename);
	void Render(ID3D11DeviceContext* deviceContext, ID3D11Buffer* constantbufferLight);
	void ShutDown();
	~DeferredShader();
private:
	struct MatrixBufferType
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;
		DirectX::XMMATRIX viewLight;
	};

	struct VerticeData
	{
		float x, y, z;
	};
	ID3D11Buffer* m_vertexBuffer;
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11SamplerState* m_sampleStateWrap, * m_sampleStateClamp;

	void InitializeShader(ID3D11Device* device, HWND shadername, WCHAR* vsFilename, WCHAR* psFilename);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob* blob, HWND errorString, WCHAR* dno);
	void SetShaderParameters(ID3D11DeviceContext* devcon, const DirectX::XMMATRIX& world, const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection);
	void RenderShader(ID3D11DeviceContext* devcon, ID3D11Buffer* constantbufferLight);
};

#endif
