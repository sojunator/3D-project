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

void DeferredShader::InitializeShader(ID3D11Device* device, HWND handle, WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT hr;
	ID3DBlob* errorMsg = 0;
	ID3DBlob* vertexShaderBuffer;
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

	ID3DBlob* pixelShaderBuffer;
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
		_CrtDbgBreak();
		MessageBox(handle, L"Failed to create input layout for vertex shader", L"CRAWLING IN MY SKIN", MB_OK);
	}

	VerticeData verts[6];

	verts[0].x = -1; // upper left corner
	verts[0].y = -1;
	verts[0].z = 0;

	verts[1].x = 1; // upper right corner
	verts[1].y = -1;
	verts[1].z = 0;

	verts[2].x = 1; // lower right corner;
	verts[2].y = 1;
	verts[2].z = 0;
	
	verts[3] = verts[2];

	verts[4].x = -1; // lower left corner;
	verts[4].y = 1;
	verts[4].z = 0;

	verts[5] = verts[0];


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

void DeferredShader::Render(ID3D11DeviceContext* devcon, int indexCount, const DirectX::XMMATRIX& world, const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection)
{
	SetShaderParameters(devcon, world, view, projection);
	RenderShader(devcon, indexCount);
}

void DeferredShader::RenderShader(ID3D11DeviceContext* devcon, int indexCount)
{
	devcon->IASetInputLayout(m_layout);
	devcon->VSSetShader(m_vertexShader, NULL, 0);
	devcon->PSSetShader(m_pixelShader, NULL, 0);
	devcon->PSSetSamplers(0, 1, &m_sampleState);
	devcon->DrawIndexed(indexCount, 0, 0);
}

DeferredShader::~DeferredShader()
{

}