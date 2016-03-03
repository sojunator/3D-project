#ifndef TERRAINCLASS_H
#define TERRAINCLASS_H

#include "defines.h"

class TerrainClass
{
public:
	struct verts 
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT4 color;
	};
	struct HeightMapType
	{
		float x, y, z;
	};
	struct ModelType
	{
		float x, y, z;
	};

	TerrainClass();
	~TerrainClass();
	void Initalize(ID3D11Device* dev, std::string setupFilename);
	void Render(ID3D11DeviceContext* devcon);
	void Shutdown();
	int GetIndexCount() { return m_indexCount; }
private:
	void Renderbuffers(ID3D11DeviceContext* devcon);
	void InitializeBuffers(ID3D11Device* device);
	ID3D11Buffer*  m_vertexBuffer, *m_indexBuffer;
	int m_vertexCount, m_indexCount;

	void LoadSetupFile(std::string filename);
	void LoadBitmapHeightMap();
	void ShutdownHeightMap();
	void SetTerrainCoordinates();
	void BuildTerrainModel();
	void ShutdownTerrainModel();

	int m_height, m_width;
	float m_heightScale;
	std::string m_terrainFileName;
	HeightMapType* m_heightMap;
	ModelType* m_terrainModel;
};

#endif
