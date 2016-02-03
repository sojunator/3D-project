#include "inc\Graphic.h"
#include <math.h>

Graphics::Graphics(HWND handle)
{
	m_DirectX = 0;
	m_Camera = 0;
	m_Model = 0;
	m_Shader = 0;

	m_DirectX = new D3DClass(handle);
	m_DirectX->Intialize();


	// Create the camera object.
	m_Camera = new CameraClass;

	// Set the initial position of the camera.
	m_Camera->SetPosition(0.0f, 0.0f, -5.0f);

	// Create the model object.
	m_Model = new Model(m_DirectX->GetDevice());


	// Create the color shader object.
	m_Shader = new ShaderClass;


	// Initialize the color shader object.
	m_Shader->Initialize(m_DirectX->GetDevice(), handle);

}

bool Graphics::Update(float dt)
{
	return true;
}

bool Graphics::Render(float dt, bool wasd[4], POINT mousePos)
{
	DirectX::XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	float posArr[3];

	m_Camera->GetPosition(posArr);

	if (wasd[0])
	{
		m_Camera->SetPosition(posArr[0], posArr[1], posArr[2] + 2*dt);
	}
	if (wasd[1])
	{
		m_Camera->SetPosition(posArr[0] + 2*dt, posArr[1], posArr[2]);
	}

	if (wasd[2])
	{
		m_Camera->SetPosition(posArr[0], posArr[1], posArr[2] - 2*dt);
	}

	if (wasd[3])
	{
		m_Camera->SetPosition(posArr[0] - 2*dt, posArr[1], posArr[2]);
	}

	m_DirectX->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_DirectX->GetProjectionMatrix(projectionMatrix);

	m_DirectX->InitScene(sinf(dt) * 0.5f, sinf(dt) * 0.3f, 0.2f, 1.0f);
	m_Camera->Render(mousePos);
	m_Model->Render(m_DirectX->GetDeviceContext());
	m_Shader->Render(m_DirectX->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix);
	m_DirectX->PresentScene();

	return true;
}

void Graphics::Shutdown()
{
	// Release the color shader object.
	if (m_Shader)
	{
		m_Shader->ShutDown();
		delete m_Shader;
		m_Shader = 0;
	}

	// Release the model object.
	if (m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
	}

	//// Release the camera object.
	//if (m_Camera)
	//{
	//	delete m_Camera;
	//	m_Camera = 0;
	//}

	//// Release the D3D object.
	//if (m_DirectX)
	//{
	//	m_DirectX->ShutDown();
	//	delete m_DirectX;
	//	m_DirectX = 0;
	//}

	return;
}

Graphics::~Graphics()
{

}