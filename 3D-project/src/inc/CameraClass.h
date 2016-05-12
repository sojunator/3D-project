#ifndef CAMERACLASS_H
#define CAMERACLASS_H

#include "defines.h"

class CameraClass
{
public:
	CameraClass();
	void SetPosition(DirectX::XMFLOAT3 newPosition);
	void SetRotation(float x, float y, float z);

	DirectX::XMFLOAT3 GetPosition();
	void GetRotation(float& arr);
	void HandleKeyInput(bool wasd[4], float dt);

	void Render(POINT mousePos);
	void GetViewMatrix(DirectX::XMMATRIX& view);
	void GetWorldMatrix(DirectX::XMMATRIX& worldMatrix);
	void SetY(float Y);
	~CameraClass();
private:

	POINT lastMousePos;
	DirectX::XMFLOAT3 m_position;
	float m_rotationX, m_rotationY, m_rotationZ;
	float yaw = 0, pitch = 0, roll = 0;
	DirectX::XMMATRIX m_viewMatrix;
	DirectX::XMMATRIX m_worldMatrix;

};

#endif
