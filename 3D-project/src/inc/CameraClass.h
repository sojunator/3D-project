#ifndef CAMERACLASS_H
#define CAMERACLASS_H

#include "defines.h"

class CameraClass
{
public:
	CameraClass();
	void SetPosition(float x, float y, float z);
	void SetRotation(float x, float y, float z);

	void GetPosition(float arr[3]);
	void GetRotation(float& arr);

	void Render(POINT mousePos);
	void GetViewMatrix(DirectX::XMMATRIX& view);
	void GetWorldMatrix(DirectX::XMMATRIX& worldMatrix);

	~CameraClass();
private:

	POINT lastMousePos;
	float m_positionX, m_positionY, m_positionZ;
	float m_rotationX, m_rotationY, m_rotationZ;
	float yaw = 0, pitch = 0, roll = 0;
	DirectX::XMMATRIX m_viewMatrix;
	DirectX::XMMATRIX m_worldMatrix;

};

#endif
