#include "inc\DeferredShader.h"
#include <string>

DeferredShader::DeferredShader()
{
	m_vertexBuffer = 0;
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

void DeferredShader::InitializeShader(ID3D11Device* device, HWND handle, WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT hr;
	ID3DBlob* errorMsg = 0;
	ID3DBlob* vertexShaderBuffer;
	hr = D3DCompileFromFile(vsFilename, NULL, NULL, "VS_main", "vs_4_0", D3DCOMPILE_DEBUG, 0, &vertexShaderBuffer, &errorMsg);
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

	ID3DBlob* pixelShaderBuffer;
	hr = D3DCompileFromFile(psFilename, NULL, NULL, "PS_main", "ps_4_0", D3DCOMPILE_DEBUG, 0, &pixelShaderBuffer, &errorMsg);
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



	D3D11_INPUT_ELEMENT_DESC polygonLayout[1];
	polygonLayout[0].SemanticName = "SV_POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;
	int numElements = sizeof(polygonLayout[0]) / sizeof(polygonLayout);
	hr = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(hr))
	{
		_CrtDbgBreak();
		MessageBox(handle, L"Failed to create input layout for vertex shader", L"CRAWLING IN MY SKIN", MB_OK);
	}

	// sloby commit
	VerticeData verts[6];

	verts[0].x = -1; // upper left corner
	verts[0].y = 1;
	verts[0].z = 0;

	verts[1].x = 1; // upper right corner
	verts[1].y = 1;
	verts[1].z = 0;

	verts[2].x = 1; // lower right corner;
	verts[2].y = -1;
	verts[2].z = 0;
	
	verts[3] = verts[2];

	verts[4].x = -1; // lower left corner;
	verts[4].y = -1;
	verts[4].z = 0;

	verts[5] = verts[0];

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
		MessageBox(NULL, L"Failed to create constant buffer", L"THESE WORMS", MB_OK);
	}


	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = &verts;
	initData.SysMemPitch = 0;
	initData.SysMemSlicePitch = 0;

	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VerticeData) * 6;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	hr = device->CreateBuffer(&vertexBufferDesc, &initData, &m_vertexBuffer);

	// We dont need the buffers anymore
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;
	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	// Create a texture sampler state description.
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;


	// Create the texture sampler state.
	hr = device->CreateSamplerState(&samplerDesc, &m_sampleState);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to create sampler on gpu", L"Sampler error", MB_OK);
	}

}

void DeferredShader::ShutDown()
{
	ShutdownShader();
}

void DeferredShader::ShutdownShader()
{
	if (m_sampleState)
	{
		m_sampleState->Release();
		m_sampleState = 0;
	}

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

	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}
}

void DeferredShader::OutputShaderErrorMessage(ID3D10Blob* errorMsg, HWND hwnd, WCHAR* shaderFilename)
{
	////OutputDebugStringA(static_cast<char*>(errorMsg->GetBufferPointer()));
	MessageBox(hwnd, L"Error compiling shader. Check output debug.", shaderFilename, MB_OK);
	//errorMsg->Release();
}

void DeferredShader::SetShaderParameters(ID3D11DeviceContext* devcon, const DirectX::XMMATRIX& world, const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection)
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
}

void DeferredShader::Render(ID3D11DeviceContext* devcon, ID3D11Buffer* constantBufferLight)
{
	RenderShader(devcon, constantBufferLight);
}

void DeferredShader::configureShader(ID3D11DeviceContext* devcon, const DirectX::XMMATRIX& world, const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection)
{
	UINT temp = 12;
	UINT zero = 0;
	SetShaderParameters(devcon, world, view, projection);
	devcon->IASetInputLayout(m_layout);
	devcon->VSSetShader(m_vertexShader, NULL, 0);
	devcon->PSSetShader(m_pixelShader, NULL, 0);
	devcon->GSSetShader(NULL, NULL, 0);

	devcon->PSSetSamplers(0, 1, &m_sampleState);
	devcon->IASetVertexBuffers(0, 1, &m_vertexBuffer, &temp, &zero);
}

void DeferredShader::RenderShader(ID3D11DeviceContext* devcon, ID3D11Buffer* constantbufferLight)
{
	devcon->PSSetConstantBuffers(0, 1, &constantbufferLight);
	devcon->Draw(6, 0);
}

DeferredShader::~DeferredShader()
{

}