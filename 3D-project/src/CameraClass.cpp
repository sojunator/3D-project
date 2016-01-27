#include "inc\defines.h"
#include "inc\CameraClass.h"

CameraClass::CameraClass()
{
	m_positionX = 0.0f;
	m_positionY = 0.0f;
	m_positionZ = 0.0f;

	m_rotationX = 0.0f;
	m_rotationY = 0.0f;
	m_rotationZ = 0.0f;
}

void CameraClass::SetPosition(float x, float y, float z)
{
	m_positionX = x;
	m_positionY = y;
	m_positionZ = z;
}

void CameraClass::SetRotation(float x, float y, float z)
{
	m_rotationX = x;
	m_rotationY = y;
	m_rotationZ = z;
}

void CameraClass::GetPosition(float arr[3])
{
	arr[0] = m_positionX;
	arr[1] = m_positionY;
	arr[2] = m_positionZ;
}


void CameraClass::Render()
{
	DirectX::XMFLOAT3 up, position, lookAt;
	DirectX::XMVECTOR upVector, positionVector, lookAtVector;
	DirectX::XMMATRIX rotationMatrix;
	float yaw, pitch, roll;

	// upwards vector
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	upVector = DirectX::XMLoadFloat3(&up);
	
	// Pos in world
	position.x = m_positionX;
	position.y = m_positionY;
	position.z = m_positionZ;

	// Load it into xmvector structure
	positionVector = DirectX::XMLoadFloat3(&position);

	// Setup where the camera is looking
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 5.0f;

	lookAtVector = DirectX::XMLoadFloat3(&lookAt);

	// Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians.
	pitch = m_rotationX * 0.0174532925f;
	yaw = m_positionY * 0.0174532925f;
	roll = m_rotationZ * 0.0174532925f;

	// Create our rotation matrix
	rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	// Transform the lookAt and up vector by rotation matrix so the view is correctly rotated at the origin.
	lookAtVector = DirectX::XMVector3TransformCoord(lookAtVector, rotationMatrix);
	upVector = DirectX::XMVector3TransformCoord(upVector, rotationMatrix);

	// Translate the rotated camera position to the location of the viewer
	lookAtVector = DirectX::XMVectorAdd(positionVector, lookAtVector);

	// Finally create the view matrix from the three updated vectors
	m_viewMatrix = DirectX::XMMatrixLookAtLH(positionVector, lookAtVector, upVector);

}

void CameraClass::GetViewMatrix(DirectX::XMMATRIX& viewMatrix)
{
	viewMatrix = m_viewMatrix;
}