#include "inc\defines.h"
#include "inc\Model.h"

Model::Model(ID3D11Device* device)
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;

	InitializeBuffers(device);
}

bool Model::InitializeBuffers(ID3D11Device* device)
{
	// Since we are just rendering a triangle, everything will be hardcoded accordingly.
	m_vertexCount = 3; 
	m_indexCount = 3;

	Vertex* vertices = new Vertex[m_vertexCount];
	unsigned long* indices = new unsigned long[m_indexCount];

	// Left bottom
	vertices[0].position = DirectX::XMFLOAT3(-1.0f, -1.0f, 0.0f);
	vertices[0].color = DirectX::XMFLOAT4(1.0f, 0.2f, 0.6f, 1.0f);

	// Top
	vertices[1].position = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertices[1].color = DirectX::XMFLOAT4(0.3f, 1.0f, 0.6f, 1.0f);

	vertices[2].position = DirectX::XMFLOAT3(1.0f, -1.0f, 0.0f);
	vertices[2].color = DirectX::XMFLOAT4(0.3f, 0.2f, 1.0f, 1.0f);

	// Configure index
	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;

	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Copy the data
	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	HRESULT hr;
	hr = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to create vertexbuffer", L"BRRRRRTT", MB_OK);
		return false;
	}

	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.StructureByteStride = 0;
	indexBufferDesc.MiscFlags = 0;

	// Copy the data
	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	hr = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to create index buffer", L"BRRRR2RTT", MB_OK);
		return false;
	}

	delete[] vertices;
	delete[] indices;

	return true;
}

void Model::RenderBuffers(ID3D11DeviceContext* devcon)
{
	unsigned int stride;
	unsigned int offset;

	stride = sizeof(Vertex);
	offset = 0;

	devcon->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	devcon->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Model::Shutdown()
{
	ShutdownBuffers();
}

void Model::ShutdownBuffers()
{
	m_indexBuffer->Release();
	m_indexBuffer = 0;

	m_vertexBuffer->Release();
	m_vertexBuffer = 0;
}

void Model::Render(ID3D11DeviceContext* devcon)
{
	RenderBuffers(devcon);
}

int Model::GetIndexCount()
{
	return m_indexCount;
}