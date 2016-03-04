#include "inc\Graphic.h"
#include "inc\Light.h"
#include "inc\DeferredShader.h"
#include <math.h>
#include <time.h>

Graphics::Graphics(HWND handle)
{
	m_DirectX = 0;
	m_Camera = 0;
	m_Model = 0;
	m_ShaderLight = 0;
	m_ComputeShader = 0;

	m_DirectX = new D3DClass(handle);
	m_DirectX->Intialize();

	m_DirectX->CreateRenderTargetViews();
	m_DirectX->CreateBlendState();

	// Create the camera object.
	m_Camera = new CameraClass;

	// Set the initial position of the camera.
	m_Camera->SetPosition(DirectX::XMFLOAT3(0.0f, 0.0f, -5.0f));

	// Create the model object.
	m_Model = new Model(m_DirectX->GetDevice());

	// Create the heighmap
	m_map = new TerrainClass();
	m_map->Initalize(m_DirectX->GetDevice(), "Setup.txt");
	

	// Create all of our shaders
	m_TerrainShader = new ShaderClass(ShaderClass::TERRAIN);
	m_Shader = new ShaderClass(ShaderClass::OBJ);
	m_ShaderLight = new DeferredShader;
	m_ComputeShader = new ComputeShader;

	// Initialize all of our shaders
	m_Shader->Initialize(m_DirectX->GetDevice(), handle, L"../3D-project/src/hlsl/1_VertexShader.hlsl", L"../3D-project/src/hlsl/1_PixelShader.hlsl", L"../3D-project/src/hlsl/1_GeometryShader.hlsl");
	m_TerrainShader->Initialize(m_DirectX->GetDevice(), handle, L"../3D-project/src/hlsl/1_terrain_VertexShader.hlsl", L"../3D-project/src/hlsl/1_terrain_PixelShader.hlsl", NULL);

	m_ShaderLight->Initialize(m_DirectX->GetDevice(), handle, L"../3D-project/src/hlsl/2_VertexShader.hlsl", L"../3D-project/src/hlsl/2_PixelShader.hlsl");
	m_ComputeShader->Initialize(m_DirectX->GetDevice(), handle, L"../3D-project/src/hlsl/1_ComputeShader.hlsl");


	// Create light array, this array handles all lights an its information
	srand(time(NULL));
	for (int i = 0; i < AMOUNT_OF_LIGHTS; i++)
	{
		DirectX::XMFLOAT3 lightPos = DirectX::XMFLOAT3(rand()% 4, rand() % 3, rand() % 4);
		DirectX::XMFLOAT4 lightColour = DirectX::XMFLOAT4(static_cast <float> (rand()) / static_cast <float> (RAND_MAX), static_cast <float> (rand()) / static_cast <float> (RAND_MAX), static_cast <float> (rand()) / static_cast <float> (RAND_MAX), 1.0f);
		float ambientStrenght = 0.1f / AMOUNT_OF_LIGHTS;
		Light tempLight = Light(lightPos, lightColour, ambientStrenght, m_Camera->GetPosition(), m_DirectX->GetDevice());
		tempLight.CreateConstantBuffer();
		m_lights.push_back(tempLight);
	}

}

bool Graphics::Update(float dt)
{
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
	m_DirectX->PrePareGeoPass();
	m_Camera->Render(mousePos);

	m_DirectX->WireFrameState();
	m_map->Render(m_DirectX->GetDeviceContext());
	m_TerrainShader->Render(m_DirectX->GetDeviceContext(), m_map->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix);

	m_DirectX->DefualtState();
	m_Model->Render(m_DirectX->GetDeviceContext());
	m_Shader->Render(m_DirectX->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix);



	// Second pass, lights
	m_DirectX->PrepareLightPass();
	m_ShaderLight->configureShader(m_DirectX->GetDeviceContext(), worldMatrix, viewMatrix, projectionMatrix);
	m_DirectX->SetBlendState();
	for (int i = 0; i < AMOUNT_OF_LIGHTS; i++)
	{
		m_ShaderLight->Render(m_DirectX->GetDeviceContext(), m_lights[i].GetConstantBuffer());
	}

	// Third pass, postprocess
	DirectX::XMFLOAT3 groups = DirectX::XMFLOAT3(20, 30, 1);
	m_DirectX->PreparePostPass();
	m_ComputeShader->Render(m_DirectX->GetDeviceContext(), groups);

	// Swap buffers
	m_DirectX->PresentScene();

	return true;
}

void Graphics::Shutdown()
{
	if (m_TerrainShader)
	{
		m_TerrainShader->ShutDown();
		delete m_TerrainShader;
		m_TerrainShader = 0;
	}

	if (m_ComputeShader)
	{
		m_ComputeShader->ShutDown();
		delete m_ComputeShader;
		m_ComputeShader = 0;
	}

	if (m_ShaderLight)
	{
		m_ShaderLight->ShutDown();
	    delete	m_ShaderLight;
		m_ShaderLight = 0;
	}

	if (m_map)
	{
		m_map->Shutdown();
		delete m_map;
		m_map = 0;
	}

	for (int i = 0; i < AMOUNT_OF_LIGHTS; i++)
	{
		m_lights[i].Shutdown();
	}

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