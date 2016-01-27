#ifndef MODEL_H
#define MODEL_H

#include "defines.h"

class Model
{
public:
	Model(ID3D11Device* device);
	void Render(ID3D11DeviceContext* devcon);
	int GetIndexCount();
	void Shutdown();

private:
    int	m_indexCount;
	int m_vertexCount;
	ID3D11Buffer* m_vertexBuffer, *m_indexBuffer;

	struct Vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT4 color;
	};

	bool InitializeBuffers(ID3D11Device* device);
	void RenderBuffers(ID3D11DeviceContext* devcon);
	void ShutdownBuffers();

};

#endif