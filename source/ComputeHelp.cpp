//--------------------------------------------------------------------------------------
// Copyright (c) Stefan Petersson 2012. All rights reserved.
//--------------------------------------------------------------------------------------
#include "ComputeHelp.h"
#include <cstdio>

ComputeShader::ComputeShader()
	: mD3DDevice(nullptr), mD3DDeviceContext(nullptr), mShader(nullptr)
{

}

ComputeShader::~ComputeShader()
{
	SAFE_RELEASE(mD3DDevice);
}

bool ComputeShader::Init(LPCWSTR shaderFile, char* blobFileAppendix, char* pFunctionName, D3D10_SHADER_MACRO* pDefines,
	ID3D11Device* d3dDevice, ID3D11DeviceContext*d3dContext)
{
	HRESULT hr = S_OK;
	mD3DDevice = d3dDevice;
	mD3DDeviceContext = d3dContext;

	ID3DBlob* pCompiledShader = nullptr;
	ID3DBlob* pErrorBlob = nullptr;
	
	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;

	/*
	#if defined(DEBUG) || defined(_DEBUG)
		dwShaderFlags |= D3DCOMPILE_DEBUG;
		dwShaderFlags |= D3DCOMPILE_OPTIMIZATION_LEVEL0;
	#else
		dwShaderFlags |= D3DCOMPILE_OPTIMIZATION_LEVEL0;
	#endif
	*/

	hr = D3DCompileFromFile(shaderFile, pDefines, D3D_COMPILE_STANDARD_FILE_INCLUDE, pFunctionName, "cs_5_0", dwShaderFlags, 0, &pCompiledShader, &pErrorBlob);
	
	if (pErrorBlob)
	{
		OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
	}

	if(hr == S_OK)
	{
		if(hr == S_OK)
		{
			hr = mD3DDevice->CreateComputeShader(pCompiledShader->GetBufferPointer(),
				pCompiledShader->GetBufferSize(), NULL, &mShader);

		}
	}

	SAFE_RELEASE(pErrorBlob);
	SAFE_RELEASE(pCompiledShader);

	return hr;
}

void ComputeShader::Set()
{
	mD3DDeviceContext->CSSetShader( mShader, NULL, 0 );
}

void ComputeShader::Unset()
{
	mD3DDeviceContext->CSSetShader( NULL, NULL, 0 );
}