//--------------------------------------------------------------------------------------
// Copyright (c) Stefan Petersson 2012. All rights reserved.
//--------------------------------------------------------------------------------------
#pragma once

#include <d3dcommon.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <tchar.h>


#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if (p) { (p)->Release(); (p)=NULL; } }
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)      { if (p) { delete (p); (p)=NULL; } }
#endif

class ComputeShader
{
public:
	ComputeShader();
	~ComputeShader();
	
	bool Init(LPCWSTR shaderFile, char* blobFileAppendix, char* pFunctionName, D3D10_SHADER_MACRO* pDefines, ID3D11Device* d3dDevice, ID3D11DeviceContext*d3dContext);

	void Set();
	void Unset();

private:
	ID3D11Device*               mD3DDevice;
	ID3D11DeviceContext*        mD3DDeviceContext;
	ID3D11ComputeShader*		mShader;
};

/*
class ComputeWrap
{

public:
	ComputeWrap(ID3D11Device* d3dDevice, ID3D11DeviceContext* d3dContext)
	{
		mD3DDevice = d3dDevice;
		mD3DDeviceContext = d3dContext;
	}

	ComputeShader* CreateComputeShader(LPCWSTR shaderFile, char* blobFileAppendix, char* pFunctionName, D3D10_SHADER_MACRO* pDefines);	

private:

	ID3D11Device*               mD3DDevice;
	ID3D11DeviceContext*        mD3DDeviceContext;
};
*/