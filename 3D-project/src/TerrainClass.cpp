#include "inc\TerrainClass.h"
#include <fstream>
#include <sstream>
#include <vector>

TerrainClass::TerrainClass()
{
	m_indexBuffer = 0;
	m_vertexBuffer = 0;
	m_heightMap = 0;
	m_terrainModel = 0;
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

	devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void TerrainClass::ShutdownHeightMap()
{
	if (m_heightMap)
	{
		delete[] m_heightMap;
		m_heightMap = 0;
	}
}

void TerrainClass::ShutdownTerrainModel()
{
	if (m_terrainModel)
	{
		delete[] m_terrainModel;
		m_terrainModel = 0;
	}
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

	ShutdownHeightMap();
	ShutdownTerrainModel();
}

void TerrainClass::LoadBitmapHeightMap()
{
	BITMAPFILEHEADER bitmapFileHeader;
	BITMAPINFOHEADER bitmapInfoHeader;

	m_heightMap = new HeightMapType[m_width * m_height];
	FILE* fileptr;
	unsigned char* bitmapImage;
	int imageSize;
	if (fopen_s(&fileptr, m_terrainFileName.c_str(), "rb") != 0)
	{
		MessageBox(NULL, L"Failed to open bitmap, prepare for crash", L"SHIET", MB_OK);
		return;
	}

	if (fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, fileptr) != 1)
	{
		MessageBox(NULL, L"Failed to read bitmap, prepare for crash", L"SHIET", MB_OK);
		return;
	};

	if (fread(&bitmapInfoHeader, sizeof(BITMAPFILEHEADER), 1, fileptr) != 1)
	{
		MessageBox(NULL, L"Failed to read bitmap, prepare for crash", L"SHIET", MB_OK);
		return;
	};

	if ((bitmapInfoHeader.biHeight != m_height) || (bitmapInfoHeader.biWidth != m_width))
	{
		MessageBox(NULL, L"Dimension mismatch for heightmap", L"Fatal error", MB_OK);
		return;
	}

	imageSize = m_height * ((m_width * 3) + 1);
	bitmapImage = new unsigned char[imageSize];

	fseek(fileptr, bitmapFileHeader.bfOffBits, SEEK_SET); // move to the start of the bitmapdata
	if (fread(bitmapImage, 1, imageSize, fileptr) != imageSize)
	{
		MessageBox(NULL, L"file size does not equal readsize", L"SHIET", MB_OK);
		return;
	}
	int index, height, k = 0;
	fclose(fileptr); // We now have the data, close the stream
	for (int i = 0; i < m_height; i++)
	{
		for (int j = 0; j < m_width; j++)
		{
			// bmps are upside down, load efterything in backwards
			index = (m_width* (m_height - 1 - i)) + j;
			height = bitmapImage[k];
			m_heightMap[index].y = (float)height;
			k += 3;
		}
		k++;
	}

	delete[] bitmapImage;
	bitmapImage = 0;
}

void TerrainClass::SetTerrainCoordinates()
{

	for (int i = 0, index = 0; i < m_height; i++)
	{
		for (int j = 0; j < m_width; j++)
		{
			index = (m_width*i) + j;
			m_heightMap[index].x = (float)j;
			m_heightMap[index].z = -(float)i;

			m_heightMap[index].z += (float)(m_height - 1);
			m_heightMap[index].y /= m_heightScale;
		}
	}
}

void TerrainClass::BuildTerrainModel()
{
	m_vertexCount = (m_height - 1)*(m_width - 1) * 6;
	m_terrainModel = new ModelType[m_vertexCount];
	int index = 0;
	for (int i = 0; i < (m_height - 1); i++)
	{
		for (int j = 0; j < (m_width - 1); j++)
		{
			int index1, index2, index3, index4;
			index1 = (m_width*i) + j;
			index2 = (m_width*i) + (j+1);
			index3 = (m_width * (i+1)) + j;
			index4 = (m_width* (i+1)) + (j+1);

			// Triangle 1 - upleft
			m_terrainModel[index].x = m_heightMap[index1].x;
			m_terrainModel[index].y = m_heightMap[index1].y;
			m_terrainModel[index].z = m_heightMap[index1].z;
			index++;

			// Triangle 1 upper right
			m_terrainModel[index].x = m_heightMap[index2].x;
			m_terrainModel[index].y = m_heightMap[index2].y;
			m_terrainModel[index].z = m_heightMap[index2].z;
			index++;

			// triangle 1 lower left
			m_terrainModel[index].x = m_heightMap[index3].x;
			m_terrainModel[index].y = m_heightMap[index3].y;
			m_terrainModel[index].z = m_heightMap[index3].z;
			index++;

			// Triangle 2 lower left
			m_terrainModel[index].x = m_heightMap[index3].x;
			m_terrainModel[index].y = m_heightMap[index3].y;
			m_terrainModel[index].z = m_heightMap[index3].z;
			index++;

			//Triangle 2 upper right
			m_terrainModel[index].x = m_heightMap[index2].x;
			m_terrainModel[index].y = m_heightMap[index2].y;
			m_terrainModel[index].z = m_heightMap[index2].z;
			index++;

			// Triangle 2 lower right
			m_terrainModel[index].x = m_heightMap[index4].x;
			m_terrainModel[index].y = m_heightMap[index4].y;
			m_terrainModel[index].z = m_heightMap[index4].z;
			index++;
		}
	}
}

void TerrainClass::LoadSetupFile(std::string filename)
{
	std::istringstream inputString;
	std::ifstream setup_file(TERRAIN_FOLDER_PATH + filename);
	std::string tempLine, special;
	while (std::getline(setup_file, tempLine))
	{
		inputString.str(tempLine);
		if (tempLine.substr(8, 3) == "fil")
		{
			inputString >> special >> m_terrainFileName;
			m_terrainFileName = TERRAIN_FOLDER_PATH + m_terrainFileName;
		}

		if (tempLine.substr(8, 3) == "hei")
		{
			inputString >> special >> m_height;
		}


		if (tempLine.substr(8, 3) == "wid")
		{
			inputString >> special >> m_width;
		}


		if (tempLine.substr(8, 3) == "sca")
		{
			inputString >> special >> m_heightScale;
		}

		inputString.clear();
	}
}

void TerrainClass::Initalize(ID3D11Device* dev, std::string setupFilename)
{
	LoadSetupFile(setupFilename);
	LoadBitmapHeightMap();
	SetTerrainCoordinates();
	BuildTerrainModel();
	ShutdownHeightMap();

	// Create buffers
	InitializeBuffers(dev);
}

void TerrainClass::InitializeBuffers(ID3D11Device* device)
{
	verts* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT hr;
	DirectX::XMFLOAT4 color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	m_indexCount =  m_vertexCount = (m_width - 1) * (m_height - 1) * 6;

	vertices = new verts[m_vertexCount];
	indices = new unsigned long[m_vertexCount];

	int index = 0;
	float positionX, positionZ;

	for (int i = 0; i < m_vertexCount; i++)
	{
		vertices[i].position = DirectX::XMFLOAT3(m_terrainModel[i].x, m_terrainModel[i].y, m_terrainModel[i].z);
		vertices[i].color = color;
		indices[i] = i;
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
