#include "inc\defines.h"
#include "inc\CameraClass.h"

CameraClass::CameraClass()
{
	m_position = DirectX::XMFLOAT3(0.f, 0.f, 0.f);

	m_rotationX = 0.0f;
	m_rotationY = 0.0f;
	m_rotationZ = 0.0f;

	lastMousePos.x = 400;
	lastMousePos.y = 300;
}

void CameraClass::SetPosition(DirectX::XMFLOAT3 newPosition)
{
	m_position = newPosition;
}

void CameraClass::SetRotation(float x, float y, float z)
{
	m_rotationX = x;
	m_rotationY = y;
	m_rotationZ = z;
}

DirectX::XMFLOAT3 CameraClass::GetPosition()
{
	return m_position;
}

void CameraClass::SetY(float Y)
{
	m_position.y = Y + 1.0f;
}

void CameraClass::Render(POINT mouseOffset)
{
	DirectX::XMFLOAT3 up, position, lookAt;
	DirectX::XMVECTOR upVector, positionVector, lookAtVector;
	DirectX::XMMATRIX rotationMatrix;
	float sensitivity = 0.0004;
	// upwards vector
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	upVector = DirectX::XMLoadFloat3(&up);
	
	// Pos in world
	position = m_position;

	// Load it into xmvector structure
	positionVector = DirectX::XMLoadFloat3(&position);

	// Setup where the camera is looking
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 5.0f;

	lookAtVector = DirectX::XMLoadFloat3(&lookAt);

	float xoffset = mouseOffset.x * sensitivity;
	float yoffset = mouseOffset.y * sensitivity;

	pitch += yoffset;
	yaw += xoffset;
	roll = m_rotationZ * 0.0174532925f;

	if (pitch > DirectX::XM_PI/2.01f) // CAN NOT BE 90 degs
		pitch = DirectX::XM_PI / 2.01f;
	if (pitch < -DirectX::XM_PI / 2.01f)
		pitch = -DirectX::XM_PI / 2.01f;

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

void CameraClass::HandleKeyInput(bool* keys, float dt)
{
	int boost = 0;
	if (keys[VK_SHIFT])
		boost = 20;

	if (keys['W'])
	{
		this->SetPosition(DirectX::XMFLOAT3(m_position.x + (2+boost) * sinf(yaw)*dt, m_position.y - (2 + boost) * sinf(pitch)*dt, m_position.z + (2 + boost) * cosf(yaw) * dt));
	}
	if (keys['A'])
	{
		this->SetPosition(DirectX::XMFLOAT3(m_position.x - (2 + boost) * dt*sinf(yaw+DirectX::XM_PI/2.0f), m_position.y, m_position.z - (2 + boost) * dt*cosf(yaw + DirectX::XM_PI / 2.0f)));
	}

	if (keys['S'])
	{
		this->SetPosition(DirectX::XMFLOAT3(m_position.x - sinf(yaw)*dt, m_position.y + (2 + boost) * sinf(pitch)*dt, m_position.z - cosf(yaw) * dt));
	}

	if (keys['D'])
	{
		this->SetPosition(DirectX::XMFLOAT3(m_position.x - (2 + boost) * dt*sinf(yaw - DirectX::XM_PI / 2.0f), m_position.y, m_position.z - (2 + boost) * dt*cosf(yaw - DirectX::XM_PI / 2.0f)));
	}
}

void CameraClass::GetViewMatrix(DirectX::XMMATRIX& viewMatrix)
{
	viewMatrix = m_viewMatrix;
}

void CameraClass::GetWorldMatrix(DirectX::XMMATRIX& worldMatrix)
{
	worldMatrix = m_worldMatrix;
}

CameraClass::~CameraClass()
{

}