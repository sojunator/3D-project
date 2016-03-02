#include "inc\TerrainClass.h"

TerrainClass::TerrainClass()
{
	m_indexBuffer = 0;
	m_vertexBuffer = 0;
}

void TerrainClass::Render(ID3D11DeviceContext* devcon)
{
	Renderbuffers(devcon);
}

TerrainClass::~TerrainClass()
{

}

void TerrainClass::Renderbuffers(ID3D11DeviceContext* devcon)
{
	unsigned int stride;
	unsigned int offset;
	stride = sizeof(verts);
	offset = 0;

	devcon->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
	devcon->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
}

void TerrainClass::Shutdown()
{
	if (m_vertexBuffer)
	{								
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}
}

void TerrainClass::Initalize(ID3D11Device* device)
{
	verts* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT hr;
	DirectX::XMFLOAT4 color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);


	int terrainHeight, terrainWidth;
	terrainHeight = terrainWidth = 256;

	m_indexCount =  m_vertexCount = (terrainWidth - 1) * (terrainWidth - 1) * 8;

	vertices = new verts[m_vertexCount];
	indices = new unsigned long[m_vertexCount];

	int index = 0;
	float positionX, positionZ;

	for (int i = 0; i < (terrainHeight - 1); i++)
	{
		for (int j = 0; j < (terrainWidth - 1); j++)
		{
			// Line 1 upper left
			positionX = (float)i;
			positionZ = (float)(j + 1);
			vertices[index].position = DirectX::XMFLOAT3(positionX, 0.0f, positionZ);
			vertices[index].color = color;
			indices[index] = index;
			index++;

			// line 1 upper right
			positionX = (float)(i + 1);
			positionZ = (float)(j + 1);
			vertices[index].position = DirectX::XMFLOAT3(positionX, 0.0f, positionZ);
			vertices[index].color = color;
			indices[index] = index;
			index++;

			// line 2 upper right
			positionX = (float)(i + 1);
			positionZ = (float)(j + 1);
			vertices[index].position = DirectX::XMFLOAT3(positionX, 0.0f, positionZ);
			vertices[index].color = color;
			indices[index] = index;
			index++;

			// line 2 bottom right
			positionX = (float)(i + 1);
			positionZ = (float)j;
			vertices[index].position = DirectX::XMFLOAT3(positionX, 0.0f, positionZ);
			vertices[index].color = color;
			indices[index] = index;
			index++;

			// line 3 bottom right;
			positionX = (float)(i + 1);
			positionZ = (float)j;
			vertices[index].position = DirectX::XMFLOAT3(positionX, 0.0f, positionZ);
			vertices[index].color = color;
			indices[index] = index;
			index++;

			// line 3 bottom left
			positionX = (float)i;
			positionZ = (float)j;
			vertices[index].position = DirectX::XMFLOAT3(positionX, 0.0f, positionZ);
			vertices[index].color = color;
			indices[index] = index;
			index++;

			// line 4 bottom left
			positionX = (float)i;
			positionZ = (float)j;
			vertices[index].position = DirectX::XMFLOAT3(positionX, 0.0f, positionZ);
			vertices[index].color = color;
			indices[index] = index;
			index++;

			// line 4 upper left;
			positionX = (float)i;
			positionZ = (float)(j + 1);
			vertices[index].position = DirectX::XMFLOAT3(positionX, 0.0f, positionZ);
			vertices[index].color = color;
			indices[index] = index;
			index++;


		}
	}
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D10_BIND_INDEX_BUFFER;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;
	indexBufferDesc.CPUAccessFlags = 0;

	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	hr = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to create indexBuffer", L"Fatal error", MB_OK);
	}

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(verts) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	hr = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to create heightmap vertexbuffer", L"Fatal error", MB_OK);
	}

	delete vertices;
	vertices = 0;

	delete indices;
	indices = 0;

}
