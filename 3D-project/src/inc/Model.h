#ifndef MODEL_H
#define MODEL_H

#include "defines.h"

class Model
{
public:
	Model(ID3D11Device* device);
	void Render(ID3D11DeviceContext* devcon, ID3D11Buffer* constantBuffer, DirectX::XMFLOAT3 CameraPosition);
	int GetIndexCount();
	void Shutdown();
	~Model();
private:
    int	m_indexCount;
	int m_vertexCount;
	ID3D11ShaderResourceView* m_pTexture;
	ID3D11Buffer* m_vertexBuffer, *m_indexBuffer;
	ID3D11Buffer* m_constantBuffer;

	struct Vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT4 normal;
		DirectX::XMFLOAT2 uvCords;
	};

	struct MaterialInfo
	{
		DirectX::XMFLOAT4 Ka;
		DirectX::XMFLOAT4 Ks;
		DirectX::XMFLOAT4 Kd;
	};

	bool InitializeBuffers(ID3D11Device* device);
	void RenderBuffers(ID3D11DeviceContext* devcon, ID3D11Buffer* constantbufferLight, DirectX::XMFLOAT3 CameraPosition);
	void ShutdownBuffers();

};

#endif