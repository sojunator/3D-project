#ifndef TERRAINCLASS_H
#define TERRAINCLASS_H

#include "defines.h"

class TerrainClass
{
public:
	struct verts {
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT4 color;
	};
	TerrainClass();
	~TerrainClass();
	void Initalize(ID3D11Device* dev);
	void Render(ID3D11DeviceContext* devcon);
	void Shutdown();
	int GetIndexCount() { return m_indexCount; }
private:
	void Renderbuffers(ID3D11DeviceContext* devcon);
	ID3D11Buffer*  m_vertexBuffer, *m_indexBuffer;
	int m_vertexCount, m_indexCount;

};

#endif
