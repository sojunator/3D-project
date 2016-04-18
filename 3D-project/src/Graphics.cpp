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
	m_GuassianShader = 0;

	m_DirectX = new D3DClass(handle);
	m_DirectX->Intialize();

	m_DirectX->CreateRenderTargetViews();
	m_DirectX->CreateBlendState();

	// Create the camera object.
	m_Camera = new CameraClass;

	// Set the initial position of the camera.
	m_Camera->SetPosition(DirectX::XMFLOAT3(10.0f, 4.5f, 2.25f));

	// Create the model object.
	m_Model = new Model(m_DirectX->GetDevice());

	// Create the heighmap
	m_map = new TerrainClass();
	m_map->Initalize(m_DirectX->GetDevice(), "Setup.txt");
	

	// Create all of our shaders
	m_TerrainShader = new ShaderClass(ShaderClass::TERRAIN);
	m_depthShaderTER = new ShaderClass(ShaderClass::TERRAIN);
	m_Shader = new ShaderClass(ShaderClass::OBJ);
	m_depthShaderOBJ = new ShaderClass(ShaderClass::OBJ);
	m_ShaderLight = new DeferredShader();
	m_GuassianShader = new ComputeShader;
	m_passThrough = new ComputeShader;

	// Initialize all of our shaders
	m_Shader->Initialize(m_DirectX->GetDevice(), handle, L"../3D-project/src/hlsl/1_VertexShader.hlsl", L"../3D-project/src/hlsl/1_PixelShader.hlsl", L"../3D-project/src/hlsl/1_GeometryShader.hlsl");
	m_TerrainShader->Initialize(m_DirectX->GetDevice(), handle, L"../3D-project/src/hlsl/1_terrain_VertexShader.hlsl", L"../3D-project/src/hlsl/1_terrain_PixelShader.hlsl", NULL);
	m_ShaderLight->Initialize(m_DirectX->GetDevice(), handle, L"../3D-project/src/hlsl/2_VertexShader.hlsl", L"../3D-project/src/hlsl/2_PixelShader.hlsl");
	m_depthShaderOBJ->Initialize(m_DirectX->GetDevice(), handle, L"../3D-project/src/hlsl/1_depth_VertexShader.hlsl", L"../3D-project/src/hlsl/1_depth_PixelShader.hlsl", NULL);
	m_depthShaderTER->Initialize(m_DirectX->GetDevice(), handle, L"../3D-project/src/hlsl/1_depth_VertexShader.hlsl", L"../3D-project/src/hlsl/1_depth_PixelShader.hlsl", NULL);
	m_GuassianShader->Initialize(m_DirectX->GetDevice(), handle, L"../3D-project/src/hlsl/1_GaussianCompute.hlsl");
	m_passThrough->Initialize(m_DirectX->GetDevice(), handle, L"../3D-project/src/hlsl/1_passThroughCompute.hlsl");

	// Create light array, this array handles all lights an its information
	srand(time(NULL));
	for (int i = 0; i < AMOUNT_OF_LIGHTS; i++)
	{
		DirectX::XMFLOAT3 lightDir = DirectX::XMFLOAT3(0.f, 0.f, 0.f);
		DirectX::XMFLOAT3 lightPos = DirectX::XMFLOAT3(10.0f, 8.5f, 2.25f);
		//DirectX::XMFLOAT4 lightColour = DirectX::XMFLOAT4(static_cast <float> (rand()) / static_cast <float> (RAND_MAX), static_cast <float> (rand()) / static_cast <float> (RAND_MAX), static_cast <float> (rand()) / static_cast <float> (RAND_MAX), 1.0f);
		DirectX::XMFLOAT4 lightColour = DirectX::XMFLOAT4(1.0f, 0.f, 0.f, 0.f);
		float ambientStrenght = 0.1f / AMOUNT_OF_LIGHTS;
		Light tempLight = Light(lightPos, lightDir, lightColour, ambientStrenght, m_Camera->GetPosition(), m_DirectX->GetDevice(), m_DirectX->GetDeviceContext());
		tempLight.CreateConstantBuffer();
		m_lights.push_back(tempLight);
	}

}

void Graphics::Update(float dt)
{

}

void Graphics::Render(float dt, bool* keys, POINT mousePos)
{
	DirectX::XMMATRIX worldMatrix, viewMatrix, projectionMatrix, translate;
	translate = DirectX::XMMatrixTranslation(3.0f, 2.0, 2.0);

	m_Camera->HandleKeyInput(keys, dt);
	m_DirectX->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_DirectX->GetProjectionMatrix(projectionMatrix);

	// First pass, geo
	m_DirectX->PrePareGeoPass();
	m_Camera->Render(mousePos);

	static bool wireframe = false;
	if (wireframe)
	{
		m_DirectX->WireFrameState();
		if (keys[VK_PRIOR])
		{
			wireframe = false;
		}
	}
	else
	{
		m_DirectX->DefualtState();
		if (keys[VK_NEXT])
		{
			wireframe = true;
		}
	}


	// First pass, render all geometry
	m_map->Render(m_DirectX->GetDeviceContext());
	m_TerrainShader->Render(m_DirectX->GetDeviceContext(), m_map->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_map->GetTexture(), m_map->GetNormal());

	m_Model->Render(m_DirectX->GetDeviceContext());
	m_Shader->Render(m_DirectX->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix*translate, viewMatrix, projectionMatrix, m_Model->GetTexture(), NULL);

	// Second pass, render from lights perspective
	m_DirectX->DefualtState();
	for (int i = 0; i < AMOUNT_OF_LIGHTS; i++)
	{
		m_lights[i].BindRtv();

		m_map->Render(m_DirectX->GetDeviceContext());
		m_depthShaderTER->Render(m_DirectX->GetDeviceContext(), m_map->GetIndexCount(), worldMatrix, m_lights[i].GetViewMatrix(), projectionMatrix, m_map->GetTexture(), NULL);

		m_Model->Render(m_DirectX->GetDeviceContext());
		m_depthShaderOBJ->Render(m_DirectX->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix*translate, m_lights[i].GetViewMatrix(), projectionMatrix, m_Model->GetTexture(), NULL);


	}


	// Third pass, lights and shadows
	m_DirectX->SetBlendState();
	m_ShaderLight->configureShader(m_DirectX->GetDeviceContext(), worldMatrix, viewMatrix, projectionMatrix);
	for (int i = 0; i < AMOUNT_OF_LIGHTS; i++)
	{
		m_DirectX->PrepareLightPass(m_lights[i].getShaderResource());
		m_ShaderLight->Render(m_DirectX->GetDeviceContext(), m_lights[i].GetConstantBuffer());
	}

	// Third pass, postprocess
	DirectX::XMFLOAT3 groups = DirectX::XMFLOAT3(20, 30, 1);
	m_DirectX->PreparePostPass();
	// If space, then blur
	if (keys[VK_SPACE])
		m_GuassianShader->Render(m_DirectX->GetDeviceContext(), groups);
	else
		m_passThrough->Render(m_DirectX->GetDeviceContext(), groups);
	// Swap buffers
	m_DirectX->PresentScene();
}

void Graphics::Shutdown()
{
	if (m_depthShaderOBJ)
	{
		m_depthShaderOBJ->ShutDown();
		delete m_depthShaderOBJ;
		m_depthShaderOBJ = 0;
	}

	if (m_depthShaderTER)
	{
		m_depthShaderTER->ShutDown();
		delete m_depthShaderTER;
		m_depthShaderTER = 0;
	}

	if (m_TerrainShader)
	{
		m_TerrainShader->ShutDown();
		delete m_TerrainShader;
		m_TerrainShader = 0;
	}


	if (m_GuassianShader)
	{
		m_GuassianShader->ShutDown();
		delete 	m_GuassianShader;
		m_GuassianShader = 0;
	}

	if (m_passThrough)
	{
		m_passThrough->ShutDown();
		delete m_passThrough;
		m_passThrough = 0;
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