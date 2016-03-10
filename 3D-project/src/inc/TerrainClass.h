#ifndef TERRAINCLASS_H
#define TERRAINCLASS_H

#include "defines.h"

class TerrainClass
{
public:
	struct verts 
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 texture;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT3 tangent;
		DirectX::XMFLOAT3 binormal;
	};
	struct HeightMapType
	{
		float x, y, z;
		float nx, ny, nz;
	};
	struct ModelType
	{
		float x, y, z;
		float tu, tv;
		float tx, ty, tz;
		float bx, by, bz;
		float nx, ny, nz;
	};

	struct Vector
	{
		float x, y, z;
	};

	TerrainClass();
	~TerrainClass();
	void Initalize(ID3D11Device* dev, std::string setupFilename);
	ID3D11ShaderResourceView* GetTexture() { return m_texture; };
	void Render(ID3D11DeviceContext* devcon);
	void Shutdown();
	int GetIndexCount() { return m_indexCount; }
private:
	void Renderbuffers(ID3D11DeviceContext* devcon);
	void InitializeBuffers(ID3D11Device* device);
	void LoadSetupFile(std::string filename);
	void LoadBitmapHeightMap();
	void ShutdownHeightMap();
	void SetTerrainCoordinates();
	void BuildTerrainModel();
	void ShutdownTerrainModel();
	void CalculateNormals();
	void CalculateTerrainVectors();
	void CalculateTangentBinormal(verts vertex1, verts vertex2, verts vertex3, Vector& tangent, Vector& binormal);

	int m_height, m_width;
	int m_vertexCount, m_indexCount;
	float m_heightScale;
	std::string m_terrainFileName;
	std::wstring m_terrainTexture, m_terrainNormalMap;
	HeightMapType* m_heightMap;
	ID3D11ShaderResourceView* m_texture;
	ID3D11Buffer*  m_vertexBuffer, *m_indexBuffer;
	ModelType* m_terrainModel;
};

#endif
