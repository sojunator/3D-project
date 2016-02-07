#include "inc\defines.h"
#include "inc\Model.h"
#include "inc\ObjLoader.h"
#include "WICTextureLoader.h"


Model::Model(ID3D11Device* device)
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;

	InitializeBuffers(device);
}

bool Model::InitializeBuffers(ID3D11Device* device)
{
	HRESULT hr;

	ObjData readData = loadObj("pyramid.obj", "pyramid.mtl");

	m_vertexCount = readData.faces.size();
	m_indexCount = readData.faces.size();

	Vertex* vertices = new Vertex[readData.faces.size()];
	unsigned long* indices = new unsigned long[m_indexCount];

	int i = 0;
	for (Face f : readData.faces)
	{
		Vertex v;

		v.position.x = readData.vertices[f.x - 1].x;
		v.position.y = readData.vertices[f.x - 1].y;
		v.position.z = readData.vertices[f.x - 1].z;

		v.uvCords.x = 1 - readData.uvCords[f.y - 1].u; // And we flip some shit
		v.uvCords.y = 1 - readData.uvCords[f.y - 1].v; // 

		v.normal.x = readData.normals[f.z - 1].x;
		v.normal.y = readData.normals[f.z - 1].y;
		v.normal.z = readData.normals[f.z - 1].z;


		vertices[i] = v;
		i++;
	}

	// Load texture
	LPCTSTR filename = L"../3D-project/src/obj/default_tex.png";
	ID3D11Resource* texture;
	hr = DirectX::CreateWICTextureFromFile(device, NULL, filename, NULL, &m_pTexture, 0);

	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to load texture", L"Error", MB_OK);
	}

	// Configure index
	for (int i = 0; i < m_indexCount; i++)
	{
		indices[i] = i;
	}

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

	devcon->PSSetShaderResources(0, 1, &m_pTexture);

	devcon->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Model::Shutdown()
{
	ShutdownBuffers();
}

Model::~Model()
{

}

void Model::ShutdownBuffers()
{
	m_indexBuffer->Release();
	m_indexBuffer = 0;

	m_vertexBuffer->Release();
	m_vertexBuffer = 0;

	m_pTexture->Release();
	m_pTexture = 0;
}

void Model::Render(ID3D11DeviceContext* devcon)
{
	RenderBuffers(devcon);
}

int Model::GetIndexCount()
{
	return m_indexCount;
}
