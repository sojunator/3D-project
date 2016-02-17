#include "inc\Graphic.h"
#include "inc\Light.h"
#include "inc\DeferredShader.h"
#include <math.h>

Graphics::Graphics(HWND handle)
{
	m_DirectX = 0;
	m_Camera = 0;
	m_Model = 0;
	m_Shader = 0;
	m_ShaderLight = 0;

	m_DirectX = new D3DClass(handle);
	m_DirectX->Intialize();

	m_DirectX->CreateRenderTargetViews();


	// Create the camera object.
	m_Camera = new CameraClass;

	// Set the initial position of the camera.
	m_Camera->SetPosition(DirectX::XMFLOAT3(0.0f, 0.0f, -5.0f));

	// Create the model object.
	m_Model = new Model(m_DirectX->GetDevice());


	// Create the color shader object.
	m_Shader = new ShaderClass;
	m_ShaderLight = new DeferredShader;
	// Initialize the color shader object.
	m_Shader->Initialize(m_DirectX->GetDevice(), handle, L"../3D-project/src/hlsl/1_VertexShader.hlsl", L"../3D-project/src/hlsl/1_PixelShader.hlsl");
	m_ShaderLight->Initialize(m_DirectX->GetDevice(), handle, L"../3D-project/src/hlsl/2_VertexShader.hlsl", L"../3D-project/src/hlsl/2_PixelShader.hlsl");

	// Create light array, this array handles all lights an its information
	DirectX::XMFLOAT3 lightPos = DirectX::XMFLOAT3(0.0f, 1.0f, -4.0f);
	DirectX::XMFLOAT4 lightColour = DirectX::XMFLOAT4(1.0f, 1.0f, 1.f, 1.0f);
	float ambientStrenght = 0.1f;
	m_lights = Light(lightPos, lightColour, ambientStrenght, m_Camera->GetPosition(), m_DirectX->GetDevice());

	m_lights.CreateConstantBuffer();


}

bool Graphics::Update(float dt)
{
	DirectX::XMFLOAT3 lightPos = DirectX::XMFLOAT3(0.0f, 6.0f, -4.0f);
	DirectX::XMFLOAT4 lightColour = DirectX::XMFLOAT4(1.0f, 1.0f, 1.f, 1.0f);
	float ambientStrenght = 0.0f;
	m_lights.updateLight(lightPos, lightColour, m_Camera->GetPosition(), TRUE);
	m_lights.CreateConstantBuffer();
	return true;
}

bool Graphics::Render(float dt, bool wasd[4], POINT mousePos)
{
	DirectX::XMMATRIX worldMatrix, viewMatrix, projectionMatrix;

	m_Camera->HandleKeyInput(wasd, dt);
	m_DirectX->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_DirectX->GetProjectionMatrix(projectionMatrix);

	// First pass, geo
	m_Camera->Render(mousePos);
	m_DirectX->SetRenderTargetViews();
	m_DirectX->InitScene(sinf(dt) * 0.5f, sinf(dt) * 0.3f, 0.2f, 1.0f);
	m_Model->Render(m_DirectX->GetDeviceContext());
	m_Shader->Render(m_DirectX->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix);

	// Second pass, lights
	m_DirectX->SetBackBuffer(); // We need to draw into the backbuffer now.
	m_DirectX->SetShaderResourceViews(); // Make the geo-data avaiable for ps
	m_ShaderLight->Render(m_DirectX->GetDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, m_lights.GetConstantBuffer());

	// Swap buffers
	m_DirectX->PresentScene();

	return true;
}

void Graphics::Shutdown()
{
	//if (m_lights) FOR lators
	//{
	//	for (int i = 0; i < nrOfLights; i++)
	//	{
	//		m_lights[i].Shutdown();
	//	}
	//	delete m_lights;
	//}

	if (m_ShaderLight)
	{
		m_ShaderLight->ShutDown();
	    delete	m_ShaderLight;
		m_ShaderLight = 0;
	}

	m_lights.Shutdown();

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

	// Release the camera object.
	if (m_Camera)
	{

		delete m_Camera;
		m_Camera = 0;
	}

	// Release the D3D object.
	if (m_DirectX)
	{
		m_DirectX->ShutDown();
		delete m_DirectX;
		m_DirectX = 0;
	}

	return;
}

Graphics::~Graphics()
{

}