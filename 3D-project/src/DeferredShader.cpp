#include "inc\DeferredShader.h"
#include <string>

DeferredShader::DeferredShader()
{
	m_vertexShader = 0;
	m_pixelShader = 0;
	m_layout = 0;
	m_matrixBuffer = 0;
	m_sampleState = 0;
}

bool DeferredShader::Initialize(ID3D11Device* device, HWND handle, WCHAR* vsFilename, WCHAR* psFilename)
{
	InitializeShader(device, handle, vsFilename, psFilename);
	return true;
}

bool DeferredShader::InitializeShader(ID3D11Device* device, HWND handle, WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT hr;
	ID3D10Blob* errorMsg = 0;
	ID3D10Blob* vertexShaderBuffer;
	hr = D3DCompileFromFile(vsFilename, NULL, NULL, "VS_main", "vs_4_0", 0, 0, &vertexShaderBuffer, &errorMsg);
	if (FAILED(hr))
	{
		if (errorMsg)
		{
			OutputDebugStringA(static_cast<char*>(errorMsg->GetBufferPointer()));
			MessageBox(handle, L"Error compiling shader. Check output debug.", vsFilename, MB_OK);
		}
		else
		{
			MessageBox(handle, vsFilename, L"Missing shader file", MB_OK);
		}
	}

	ID3D10Blob* pixelShaderBuffer;
	hr = D3DCompileFromFile(psFilename, NULL, NULL, "PS_main", "ps_4_0", 0, 0, &pixelShaderBuffer, &errorMsg);
	if (FAILED(hr))
	{
		if (errorMsg)
		{
			MessageBox(handle, L"Error compiling shader. Check output debug.", psFilename, MB_OK);
			OutputDebugStringA(static_cast<char*>(errorMsg->GetBufferPointer()));
		}
		else
		{
			MessageBox(handle, psFilename, L"Missing shader file", MB_OK);
		}
	}

	hr = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(hr))
	{
		MessageBox(handle, L"Failed to create vertex shader", L"Do anybody read this?", MB_OK);
	}

	hr = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(hr))
	{
		MessageBox(handle, L"Failed to create pixel shader", L"Do anybody read this?", MB_OK);
	}

	// We dont need the buffers anymore
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;
	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	// Create input layout
	D3D11_INPUT_ELEMENT_DESC polygonLayout;
	polygonLayout.SemanticName = "SV_POSITION";
	polygonLayout.SemanticIndex = 0;
	polygonLayout.Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout.InputSlot = 0;
	polygonLayout.AlignedByteOffset = 0;
	polygonLayout.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout.InstanceDataStepRate = 0;

	int numElements = sizeof(polygonLayout) / sizeof(polygonLayout);
	hr = device->CreateInputLayout(&polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(hr))
	{
		MessageBox(handle, L"Failed to create input layout for vertex shader", L"CRAWLING IN MY SKIN", MB_OK);
	}

	// We dont need the buffers anymore
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;
	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;




	return true;
}