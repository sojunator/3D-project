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
	m_Camera->SetPosition(DirectX::XMFLOAT3(7, 10.f, 3));

	// Create the model object.
	m_Model = new Model(m_DirectX->GetDevice());

	// Create the heighmap
	m_map = new TerrainClass();
	m_map->Initalize(m_DirectX->GetDevice(), "Setup.txt");
	

	// Create all of our shaders
	m_TerrainShader = new ShaderClass(ShaderClass::TERRAIN);
	m_Shader = new ShaderClass(ShaderClass::OBJ);
	m_ShaderLight = new DeferredShader();
	m_GuassianShader = new ComputeShader;
	m_passThrough = new ComputeShader;
	m_depthShader = new ShaderClass(ShaderClass::OBJ);

	// Initialize all of our shaders
	m_Shader->Initialize(m_DirectX->GetDevice(), handle, L"../3D-project/src/hlsl/1_VertexShader.hlsl", L"../3D-project/src/hlsl/1_PixelShader.hlsl", L"../3D-project/src/hlsl/1_GeometryShader.hlsl");
	m_TerrainShader->Initialize(m_DirectX->GetDevice(), handle, L"../3D-project/src/hlsl/1_terrain_VertexShader.hlsl", L"../3D-project/src/hlsl/1_terrain_PixelShader.hlsl", NULL);
	m_ShaderLight->Initialize(m_DirectX->GetDevice(), handle, L"../3D-project/src/hlsl/2_VertexShader.hlsl", L"../3D-project/src/hlsl/2_PixelShader.hlsl");
	m_GuassianShader->Initialize(m_DirectX->GetDevice(), handle, L"../3D-project/src/hlsl/1_GaussianCompute.hlsl");
	m_passThrough->Initialize(m_DirectX->GetDevice(), handle, L"../3D-project/src/hlsl/1_passThroughCompute.hlsl");
	m_depthShader->Initialize(m_DirectX->GetDevice(), handle, L"../3D-project/src/hlsl/1_depthVertex.hlsl", NULL, NULL);

	// Create light array, this array handles all lights an its information
	srand(time(NULL));
	for (int i = 0; i < AMOUNT_OF_LIGHTS; i++)
	{
		DirectX::XMFLOAT3 lightPos = DirectX::XMFLOAT3(3.0f, 12.0f, 0.0f);
		DirectX::XMFLOAT4 lightColour = DirectX::XMFLOAT4(1.0f, 1.f, 1.1f, 1.0f);
		float ambientStrenght = 0.3f / AMOUNT_OF_LIGHTS;
		Light tempLight = Light(DirectX::XMFLOAT3(3.0f, 2.0f, 7.5f), lightPos, lightColour, ambientStrenght, m_Camera->GetPosition(), m_DirectX->GetDevice());
		tempLight.CreateConstantBuffer();
		m_lights.push_back(tempLight);
	}

}

bool Graphics::Update(float dt)
{
	
	return true;
}

bool Graphics::Render(float dt, bool* keys, POINT mousePos)
{
	DirectX::XMMATRIX worldMatrix, viewMatrix, projectionMatrix, translate, rotate;

	m_Camera->HandleKeyInput(keys, dt);

	if (keys[0x46])
	{
		float test = m_map->GetYcord(m_Camera->GetPosition());
		m_Camera->SetY(test);
	}

	m_DirectX->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_DirectX->GetProjectionMatrix(projectionMatrix);


	
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

	translate = DirectX::XMMatrixTranslation(3.0f, 2.0f, 7.0f);
	static float rotation;
	rotation += 0.01f;
	float rads = (3.14156 / 180.0f) * rotation;
	rotate = DirectX::XMMatrixRotationY(rads);
	// First pass, geo
	m_DirectX->PrePareGeoPass();
	m_map->Render(m_DirectX->GetDeviceContext());
	m_TerrainShader->Render(m_DirectX->GetDeviceContext(), m_map->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_map->GetTexture(), m_map->GetNormal());


	m_Model->Render(m_DirectX->GetDeviceContext());
	m_Shader->Render(m_DirectX->GetDeviceContext(), m_Model->GetIndexCount(), rotate*worldMatrix*translate, viewMatrix, projectionMatrix, m_Model->GetTexture(), NULL);


	// Second pass, from lights perspective
	m_DirectX->PrepareDepthPass();
	m_Model->Render(m_DirectX->GetDeviceContext());
	m_depthShader->Render(m_DirectX->GetDeviceContext(), m_Model->GetIndexCount(), rotate*worldMatrix*translate, m_lights[0].GetLightView(), projectionMatrix, m_Model->GetTexture(), NULL);

	m_map->Render(m_DirectX->GetDeviceContext());
	m_depthShader->Render(m_DirectX->GetDeviceContext(), m_map->GetIndexCount(), worldMatrix, m_lights[0].GetLightView(), projectionMatrix, m_Model->GetTexture(), NULL);

	// Third pass, lights
	m_DirectX->DefualtState();
	m_DirectX->PrepareLightPass();
	m_ShaderLight->configureShader(m_DirectX->GetDeviceContext(), worldMatrix, viewMatrix, projectionMatrix);
	m_DirectX->SetBlendState();
	for (int i = 0; i < AMOUNT_OF_LIGHTS; i++)
	{
		m_ShaderLight->Render(m_DirectX->GetDeviceContext(), m_lights[i].GetConstantBuffer());
	}

	// Fourth pass, postprocess
	DirectX::XMFLOAT3 groups = DirectX::XMFLOAT3(20, 30, 1);
	m_DirectX->PreparePostPass();
	// If space, then blur
	if (keys[VK_SPACE])
		m_GuassianShader->Render(m_DirectX->GetDeviceContext(), groups);
	else
		m_passThrough->Render(m_DirectX->GetDeviceContext(), groups);
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

	if (m_depthShader)
	{
		m_depthShader->ShutDown();
		delete m_depthShader;
		m_depthShader = 0;
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