#include "inc\ComputeShader.h"

ComputeShader::ComputeShader()
{
	m_computeShader = 0;
}

void ComputeShader::Initialize(ID3D11Device* device, HWND handle, WCHAR* csFilename)
{
	InitializeShader(device, handle, csFilename);
}

void ComputeShader::InitializeShader(ID3D11Device* device, HWND handle, WCHAR* csFilename)
{
	HRESULT hr;
	ID3DBlob* errorMsg = 0;
	ID3DBlob* computerBuffer = 0;

	hr = D3DCompileFromFile(csFilename, NULL, NULL, "CS_main", "cs_4_0", 0, 0, &computerBuffer, &errorMsg);
	if (FAILED(hr))
	{
		if (errorMsg)
		{
			OutputDebugStringA(static_cast<char*>(errorMsg->GetBufferPointer()));
			MessageBox(handle, L"Error compiling shader. Check output debug.", csFilename, MB_OK);
		}
		else
		{
			MessageBox(handle, csFilename, L"Missing shader file", MB_OK);
		}
	}

	hr = device->CreateComputeShader(computerBuffer->GetBufferPointer(), computerBuffer->GetBufferSize(), nullptr, &m_computeShader);
	if (FAILED(hr))
	{
		MessageBox(handle, L"Failed to create computer shader", L"Fatal error", MB_OK);
	}
	errorMsg->Release();
	computerBuffer->Release();
}

void ComputeShader::ShutDown()
{
	if (m_computeShader)
	{
		m_computeShader->Release();
		m_computeShader = 0;
	}
}

void ComputeShader::Render(ID3D11DeviceContext* devcon, DirectX::XMFLOAT3 groups)
{
	RenderShader(devcon, groups);
}

void ComputeShader::RenderShader(ID3D11DeviceContext* devcon, DirectX::XMFLOAT3 groups)
{
	devcon->Dispatch((UINT)groups.x, (UINT)groups.y, (UINT)groups.z);
}