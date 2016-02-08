#ifndef MODEL_H
#define MODEL_H

#include "defines.h"
#include "CameraClass.h"

class Model
{
public:
	Model(ID3D11Device* device, CameraClass* camera);
	void Render(ID3D11DeviceContext* devcon);
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

	struct CBuffer //might change back to MaterialInfo & add separate buffer for lights
	{
		DirectX::XMFLOAT4 Ka;
		DirectX::XMFLOAT4 Ks;
		DirectX::XMFLOAT4 Kd;
		DirectX::XMFLOAT4 camPos;
		DirectX::XMFLOAT4 LightPosition;
	};

	bool InitializeBuffers(ID3D11Device* device, CameraClass* camera);
	void RenderBuffers(ID3D11DeviceContext* devcon);
	void ShutdownBuffers();

};

#endif