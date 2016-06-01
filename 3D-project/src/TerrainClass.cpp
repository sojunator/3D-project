#include "inc\TerrainClass.h"
#include "WICTextureLoader.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <math.h>

float TerrainClass::GetYcord(DirectX::XMFLOAT3 pos)
{
	for (int i = 0; i < m_height; i++)
	{
		for (int j = 0; j < m_width; j++)
		{
			int index = (m_width*i) + j;
			if (m_heightMap[index].x == int(pos.x) && m_heightMap[index].z == int(pos.z))
				return m_heightMap[index].y;
		}
	}

}

TerrainClass::TerrainClass()
{
	m_indexBuffer = 0;
	m_vertexBuffer = 0;
	m_heightMap = 0;
	m_terrainModel = 0;
	m_texture = 0;
	m_normalMap = 0;
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

	if (m_texture)
	{
		m_texture->Release();
		m_texture = 0;
	}

	if (m_normalMap)
	{
		m_normalMap->Release();
		m_normalMap = 0;
	}

	ShutdownHeightMap();
	ShutdownTerrainModel();
}

void TerrainClass::CalculateTerrainVectors()
{
	int faceCount, index;
	Vector tangent, binormal;
	verts vertex1, vertex2, vertex3;

	faceCount = m_vertexCount / 3;
	index = 0;

	for (int i = 0; i < faceCount; i++)
	{
		vertex1.position.x = m_terrainModel[index].x;
		vertex1.position.y = m_terrainModel[index].y;
		vertex1.position.z = m_terrainModel[index].z;
		vertex1.texture.x = m_terrainModel[index].tu;
		vertex1.texture.y = m_terrainModel[index].tv;
		vertex1.normal.x = m_terrainModel[index].nx;
		vertex1.normal.y = m_terrainModel[index].ny;
		vertex1.normal.z = m_terrainModel[index].nz;
		index++;

		vertex2.position.x = m_terrainModel[index].x;
		vertex2.position.y = m_terrainModel[index].y;
		vertex2.position.z = m_terrainModel[index].z;
		vertex2.texture.x = m_terrainModel[index].tu;
		vertex2.texture.y = m_terrainModel[index].tv;
		vertex2.normal.x = m_terrainModel[index].nx;
		vertex2.normal.y = m_terrainModel[index].ny;
		vertex2.normal.z = m_terrainModel[index].nz;
		index++;

		vertex3.position.x = m_terrainModel[index].x;
		vertex3.position.y = m_terrainModel[index].y;
		vertex3.position.z = m_terrainModel[index].z;
		vertex3.texture.x = m_terrainModel[index].tu;
		vertex3.texture.y = m_terrainModel[index].tv;
		vertex3.normal.x = m_terrainModel[index].nx;
		vertex3.normal.y = m_terrainModel[index].ny;
		vertex3.normal.z = m_terrainModel[index].nz;
		index++;

		CalculateTangentBinormal(vertex1, vertex2, vertex3, tangent, binormal);
		// Now we have the data for the face
		m_terrainModel[index - 1].tx = tangent.x;
		m_terrainModel[index - 1].ty = tangent.y;
		m_terrainModel[index - 1].tz = tangent.z;
		m_terrainModel[index - 1].bx = binormal.x;
		m_terrainModel[index - 1].by = binormal.y;
		m_terrainModel[index - 1].bz = binormal.z;

		m_terrainModel[index - 2].tx = tangent.x;
		m_terrainModel[index - 2].ty = tangent.y;
		m_terrainModel[index - 2].tz = tangent.z;
		m_terrainModel[index - 2].bx = binormal.x;
		m_terrainModel[index - 2].by = binormal.y;
		m_terrainModel[index - 2].bz = binormal.z;

		m_terrainModel[index - 3].tx = tangent.x;
		m_terrainModel[index - 3].ty = tangent.y;
		m_terrainModel[index - 3].tz = tangent.z;
		m_terrainModel[index - 3].bx = binormal.x;
		m_terrainModel[index - 3].by = binormal.y;
		m_terrainModel[index - 3].bz = binormal.z;
	}

}

void TerrainClass::CalculateTangentBinormal(verts vertex1, verts vertex2, verts vertex3, Vector& tangent, Vector& binormal)
{
	float vector1[3], vector2[3];
	float tuVector[2], tvVector[2];
	float den;
	float len;

	vector1[0] = vertex2.position.x - vertex1.position.x;
	vector1[1] = vertex2.position.y - vertex1.position.y;
	vector1[2] = vertex2.position.z - vertex1.position.z;

	vector2[0] = vertex3.position.x - vertex1.position.x;
	vector2[1] = vertex3.position.y - vertex1.position.y;
	vector2[2] = vertex3.position.z - vertex1.position.z;

	tuVector[0] = vertex2.texture.x - vertex1.texture.x;
	tuVector[1] = vertex3.texture.x - vertex1.texture.x;

	tvVector[0] = vertex2.texture.y - vertex1.texture.y;
	tvVector[1] = vertex3.texture.y - vertex1.texture.y;

	// calculate the inverse matrix for uv
	den = 1.0f / (tuVector[0]*tvVector[1] - tuVector[1]*tvVector[0]);

	tangent.x = (tvVector[1] * vector1[0] - tvVector[0] * vector2[0]) * den;
	tangent.y = (tvVector[1] * vector1[1] - tvVector[0] * vector2[1]) * den;
	tangent.z = (tvVector[1] * vector1[2] - tvVector[0] * vector2[2]) * den;

	binormal.x = (tuVector[0] * vector2[0] - tuVector[1] * vector1[0]) * den;
	binormal.y = (tuVector[0] * vector2[1] - tuVector[1] * vector1[1]) * den;
	binormal.z = (tuVector[0] * vector2[2] - tuVector[1] * vector1[2]) * den;

	len = sqrtf((tangent.x * tangent.x) + (tangent.y * tangent.y) + (tangent.z * tangent.z));
	tangent.x /= len;
	tangent.y /= len;
	tangent.z /= len;

	len = sqrtf((binormal.x*binormal.x) + (binormal.y * binormal.y) + (binormal.z*binormal.z));
	binormal.x /= len;
	binormal.y /= len;
	binormal.z /= len;
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

void TerrainClass::CalculateNormals()
{
	int index1, index2, index3, index;
	float vertex1[3], vertex2[3], vertex3[3], vector1[3], vector2[3], len;
	Vector* normals = new Vector[(m_width - 1) * (m_height - 1)];
	for (int i = 0; i < (m_height - 1); i++)
	{
		for (int j = 0; j < (m_width - 1); j++)
		{
			index1 = ((i + 1) * m_width) + j; // bottom left
			index2 = ((i + 1) * m_width) + (j + 1); // bottom right
			index3 = (i* m_width) + j; // upper left

			// Load the vertices for the face
			vertex1[0] = m_heightMap[index1].x;
			vertex1[1] = m_heightMap[index1].y;
			vertex1[2] = m_heightMap[index1].z;

			vertex2[0] = m_heightMap[index2].x;
			vertex2[1] = m_heightMap[index2].y;
			vertex2[2] = m_heightMap[index2].z;

			vertex3[0] = m_heightMap[index3].x;
			vertex3[1] = m_heightMap[index3].y;
			vertex3[2] = m_heightMap[index3].z;

			// Calculate vector values, one dim at a time.
			vector1[0] = (vertex1[0] - vertex3[0]);
			vector1[1] = (vertex1[1] - vertex3[1]);
			vector1[2] = (vertex1[2] - vertex3[2]);
			vector2[0] = (vertex3[0] - vertex2[0]);
			vector2[1] = (vertex3[1] - vertex2[1]);
			vector2[2] = (vertex3[2] - vertex2[2]);

			index = (i * (m_width - 1)) + j;

			normals[index].x = (vector1[1] * vector2[2] - vector1[2] * vector2[1]);
			normals[index].y = (vector1[2] * vector2[0] - vector1[0] * vector2[2]);
			normals[index].z = (vector1[0] * vector2[1] - vector1[1] * vector2[0]);

			// Normalize
			len = (float)sqrt(((normals[index].x*normals[index].x) + (normals[index].y * normals[index].y) + (normals[index].z * normals[index].z)));
			normals[index].x = normals[index].x / len;
			normals[index].y = normals[index].y / len;
			normals[index].z = normals[index].z / len;

		}
	}
	// now to find an average with neighbouring verts
	float sum[3];
	for (int i = 0; i < m_height; i++)
	{
		for (int j = 0; j < m_width; j++)
		{
			sum[0] = 0.0f;
			sum[1] = 0.0f;
			sum[2] = 0.0f;

			// bottom left face

			if (((j - 1) >= 0) && ((i - 1) >= 0))
			{
				index = ((i - 1) * (m_width - 1)) + (j - 1);

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
			}
			// bottom right face
			if ((j < (m_width - 1)) && ((i - 1) >= 0))
			{
				index = ((i - 1) * (m_width - 1)) + j;
				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
			}
			// upper left face
			if (((j - 1) >= 0) && (i < (m_height - 1)))
			{
				index = (i * (m_width - 1)) + (j - 1);
				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
			}
			// upper  right face

			if ((j < (m_width - 1)) && (i < (m_height - 1)))
			{
				index = (i * (m_width - 1)) + j;
				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
			}

			len = (float)sqrt((sum[0] * sum[0] + sum[1] * sum[1] + sum[2] * sum[2]));
			index = (i * m_width) + j;

			m_heightMap[index].nx = (sum[0] / len);
			m_heightMap[index].ny = (sum[1] / len);
			m_heightMap[index].nz = (sum[2] / len);

		}
	}
	delete[] normals;
	normals = 0;
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
			m_terrainModel[index].tu = 0.0f;
			m_terrainModel[index].tv = 0.0f;
			m_terrainModel[index].nx = m_heightMap[index1].nx;
			m_terrainModel[index].ny = m_heightMap[index1].ny;
			m_terrainModel[index].nz = m_heightMap[index1].nz;
			index++;

			// Triangle 1 upper right
			m_terrainModel[index].x = m_heightMap[index2].x;
			m_terrainModel[index].y = m_heightMap[index2].y;
			m_terrainModel[index].z = m_heightMap[index2].z;
			m_terrainModel[index].tu = 1.0f;
			m_terrainModel[index].tv = 0.0f;
			m_terrainModel[index].nx = m_heightMap[index2].nx;
			m_terrainModel[index].ny = m_heightMap[index2].ny;
			m_terrainModel[index].nz = m_heightMap[index2].nz;
			index++;

			// triangle 1 lower left
			m_terrainModel[index].x = m_heightMap[index3].x;
			m_terrainModel[index].y = m_heightMap[index3].y;
			m_terrainModel[index].z = m_heightMap[index3].z;
			m_terrainModel[index].tu = 0.0f;
			m_terrainModel[index].tv = 1.0f;
			m_terrainModel[index].nx = m_heightMap[index3].nx;
			m_terrainModel[index].ny = m_heightMap[index3].ny;
			m_terrainModel[index].nz = m_heightMap[index3].nz;
			index++;

			// Triangle 2 lower left
			m_terrainModel[index].x = m_heightMap[index3].x;
			m_terrainModel[index].y = m_heightMap[index3].y;
			m_terrainModel[index].z = m_heightMap[index3].z;
			m_terrainModel[index].tu = 0.0f;
			m_terrainModel[index].tv = 1.0f;
			m_terrainModel[index].nx = m_heightMap[index3].nx;
			m_terrainModel[index].ny = m_heightMap[index3].ny;
			m_terrainModel[index].nz = m_heightMap[index3].nz;
			index++;

			//Triangle 2 upper right
			m_terrainModel[index].x = m_heightMap[index2].x;
			m_terrainModel[index].y = m_heightMap[index2].y;
			m_terrainModel[index].z = m_heightMap[index2].z;
			m_terrainModel[index].tu = 1.0f;
			m_terrainModel[index].tv = 0.0f;
			m_terrainModel[index].nx = m_heightMap[index2].nx;
			m_terrainModel[index].ny = m_heightMap[index2].ny;
			m_terrainModel[index].nz = m_heightMap[index2].nz;
			index++;

			// Triangle 2 lower right
			m_terrainModel[index].x = m_heightMap[index4].x;
			m_terrainModel[index].y = m_heightMap[index4].y;
			m_terrainModel[index].z = m_heightMap[index4].z;
			m_terrainModel[index].tu = 1.0f;
			m_terrainModel[index].tv = 1.0f;
			m_terrainModel[index].nx = m_heightMap[index4].nx;
			m_terrainModel[index].ny = m_heightMap[index4].ny;
			m_terrainModel[index].nz = m_heightMap[index4].nz;
			index++;
		}
	}
}

void TerrainClass::LoadSetupFile(std::string filename)
{
	std::istringstream inputString;
	std::ifstream setup_file(TERRAIN_FOLDER_PATH + filename);
	std::string tempLine, special, tempTexture;
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

		if (tempLine.substr(8, 3) == "tex")
		{
			inputString >> special >> tempTexture;
			tempTexture = TERRAIN_FOLDER_PATH + tempTexture;
			m_terrainTexture = std::wstring(tempTexture.begin(), tempTexture.end());
		}

		if (tempLine.substr(8, 3) == "nor")
		{
			inputString >> special >> tempTexture;
			tempTexture = TERRAIN_FOLDER_PATH + tempTexture;
			m_terrainNormalMap = std::wstring(tempTexture.begin(), tempTexture.end());
		}

		inputString.clear();
	}
}

void TerrainClass::Initalize(ID3D11Device* dev, std::string setupFilename)
{
	LoadSetupFile(setupFilename);
	LoadBitmapHeightMap();
	SetTerrainCoordinates();
	CalculateNormals();
	BuildTerrainModel();
	CalculateTerrainVectors();
	//ShutdownHeightMap();

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
		vertices[i].texture = DirectX::XMFLOAT2(m_terrainModel[i].tu, m_terrainModel[i].tv);
		vertices[i].normal = DirectX::XMFLOAT3(m_terrainModel[i].nx, m_terrainModel[i].ny, m_terrainModel[i].nz);
		vertices[i].tangent = DirectX::XMFLOAT3(m_terrainModel[i].tx, m_terrainModel[i].ty, m_terrainModel[i].tz);
		vertices[i].binormal = DirectX::XMFLOAT3(m_terrainModel[i].bx, m_terrainModel[i].by, m_terrainModel[i].bz);
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

	wchar_t* fuckasci = const_cast<wchar_t*> (m_terrainTexture.c_str());
	hr = DirectX::CreateWICTextureFromFile(device, NULL, fuckasci, NULL, &m_texture, 0);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to load texture for heightmap", L"Fatal error", MB_OK);
	}

	fuckasci = const_cast<wchar_t*> (m_terrainNormalMap.c_str());
	hr = DirectX::CreateWICTextureFromFile(device, NULL, fuckasci, NULL, &m_normalMap, 0);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to load normalMap", L"Fatal error", MB_OK);
	}


}
