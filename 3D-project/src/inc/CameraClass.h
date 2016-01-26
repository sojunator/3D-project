#ifndef CAMERACLASS_H
#define CAMERACLASS_H

#include "defines.h"

class CameraClass
{
public:
	CameraClass();
	void SetPosition(float x, float y, float z);
	void SetRotation(float x, float y, float z);

	DirectX::XMFLOAT3 GetPosition();
	DirectX::XMFLOAT3 GetRotation();

	void Render();
	void GetViewMatrix(DirectX::XMMATRIX& view);

private:
	~CameraClass();
	float m_positionX, m_positionY, m_positionZ;
	float m_rotationX, m_rotationY, m_rotationZ;
	DirectX::XMMATRIX m_viewMatrix;

};

#endif
