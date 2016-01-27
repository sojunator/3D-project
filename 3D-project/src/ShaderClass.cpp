#include "inc\ShaderClass.h"
#include "inc\defines.h"
#include <string>

ShaderClass::ShaderClass()
{
	m_vertexShader = 0;
	m_pixelShader = 0;
	m_layout = 0;
	m_matrixBuffer = 0;
}

bool ShaderClass::Initialize(ID3D11Device* device, HWND handle)
{
	bool result;
	result = InitializeShader(device, handle, L"../3D-project/src/hlsl/VertexShader.hlsl", L"../3D-project/src/hlsl/PixelShader.hlsl");
	if (!result)
		return false;
	return true;
}


bool ShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
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
			MessageBox(hwnd, L"Error compiling shader. Check output debug.", vsFilename, MB_OK);
		}
		else
		{
			MessageBox(hwnd, vsFilename, L"Missing shader file", MB_OK);
		}


	}

	ID3D10Blob* pixelShaderBuffer;
	hr = D3DCompileFromFile(psFilename, NULL, NULL, "PS_main", "ps_4_0", 0, 0, &pixelShaderBuffer, &errorMsg);
	if (FAILED(hr))
	{
		if (errorMsg)
		{
			MessageBox(hwnd, L"Error compiling shader. Check output debug.", psFilename, MB_OK);
			OutputDebugStringA(static_cast<char*>(errorMsg->GetBufferPointer()));
		}
		else
		{
			MessageBox(hwnd, psFilename, L"Missing shader file", MB_OK);
		}

		
	}

	hr = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(hr))
	{
		MessageBox(hwnd, L"Failed to create vertex shader", L"Do anybody read this?", MB_OK);
	}

	hr = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(hr))
	{
		MessageBox(hwnd, L"Failed to create pixel shader", L"Do anybody read this?", MB_OK);
	}

	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	polygonLayout[0].SemanticName = "SV_POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "COLOR";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	int numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);
	hr = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(hr))
	{
		MessageBox(hwnd, L"Failed to create input layout for vertex shader", L"CRAWLING IN MY SKIN", MB_OK);
	}

	// Why dont need the buffers anymore
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;
	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	hr = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(hr))
	{
		MessageBox(hwnd, L"Failed to create constant buffer", L"THESE WORMS", MB_OK);
	}


	return true;

}

void ShaderClass::ShutdownShader()
{
	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}

	if (m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = 0;
	}

	return;
}

void ShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMsg, HWND hwnd, WCHAR* shaderFilename)
{
	////OutputDebugStringA(static_cast<char*>(errorMsg->GetBufferPointer()));
	MessageBox(hwnd, L"Error compiling shader. Check output debug.", shaderFilename, MB_OK);
	//errorMsg->Release();
}

bool ShaderClass::SetShaderParameters(ID3D11DeviceContext* devcon, const DirectX::XMMATRIX& world, const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection)
{
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	unsigned int bufferNumber;

	DirectX::XMMATRIX _world = DirectX::XMMatrixTranspose(world);
	DirectX::XMMATRIX _view = DirectX::XMMatrixTranspose(view);
	DirectX::XMMATRIX _projection = DirectX::XMMatrixTranspose(projection);
	
	hr = devcon->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(hr))
		MessageBox(NULL, L"Failed to map resouce", L"IM A SCATMAN", MB_OK);

	dataPtr = (MatrixBufferType*)mappedResource.pData;

	dataPtr->world = DirectX::XMMatrixTranspose(world);
	dataPtr->view = DirectX::XMMatrixTranspose(view);
	dataPtr->projection = DirectX::XMMatrixTranspose(projection);
	
	devcon->Unmap(m_matrixBuffer, 0);

	bufferNumber = 0;
	devcon->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);
	
	return true;
}

void ShaderClass::RenderShader(ID3D11DeviceContext* devcon, int indexCount)
{
	devcon->IASetInputLayout(m_layout);
	devcon->VSSetShader(m_vertexShader, NULL, 0);
	devcon->PSSetShader(m_pixelShader, NULL, 0);
	devcon->DrawIndexed(indexCount, 0, 0);
}

void ShaderClass::ShutDown()
{
	ShutdownShader();
}

bool ShaderClass::Render(ID3D11DeviceContext* devcon, int indexCount, const DirectX::XMMATRIX& world, const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection)
{
	bool result;

	result = SetShaderParameters(devcon, world, view, projection);
	if (!result)
		MessageBox(NULL, L"Failed to set shader params", L"Shit son", MB_OK);

	RenderShader(devcon, indexCount);

	return true;
}
