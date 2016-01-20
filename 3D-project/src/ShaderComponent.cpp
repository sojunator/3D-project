#include "inc\ShaderComponent.h"

ShaderComponent::ShaderComponent(LPCWSTR shadername, LPCSTR shadermain, LPCSTR shadertype)
{
	HRESULT hr;
	hr = D3DCompileFromFile(
		shadername, // filename
		nullptr,		// optional macros
		nullptr,		// optional include files
		shadermain,		// entry point
		shadertype,		// shader model (target)
		0,				// shader compile options
		0,				// effect compile options
		&shader,	    // double pointer to ID3DBlob		
		&error		    // pointer for Error Blob messages.
						// how to use the Error blob, see here
						// https://msdn.microsoft.com/en-us/library/windows/desktop/hh968107(v=vs.85).aspx
		);

	if (FAILED(hr) || error)
	{

		// Combines shadername with z, and then x, to create a informative errormsg
		LPCWSTR x = L"The";
		LPCWSTR z = L"shader failed to compile";
		std::wstring xshadernamez = std::wstring(x) + shadername + z;
		LPCWSTR errorString = xshadernamez.c_str();

		MessageBox(NULL, errorString, L"Error", MB_OK);
		if (error)
		{
			OutputDebugStringA(static_cast<char*>(error->GetBufferPointer()));
		}
	}

	switch (shadertype[0])
	{
	case 'p':
		
		break;
	}

}