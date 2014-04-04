//--------------------------------------------------------------------------------------
// Copyright (c) Stefan Petersson 2012. All rights reserved.
//--------------------------------------------------------------------------------------
#pragma once

#include "stdafx.h"
#include <d3dcommon.h>
#include <d3dcompiler.h>
#include <tchar.h>


class ComputeShader
{
public:
	ComputeShader();
	~ComputeShader();
	
	bool Init(LPCWSTR shaderFile, char* blobFileAppendix, char* pFunctionName, D3D10_SHADER_MACRO* pDefines, ID3D11Device* d3dDevice, ID3D11DeviceContext*d3dContext);

	// In the end it was quite the foolish decision to remove all other functions, however, I think it was necessary to do when for me during the learning part.
	// Noticed, the hard way, that I had not paid very much attention in the two 3d_courses two years ago.
	// Took me almost like 5-6 times more time to complete this project because I had to read up on everything.
	// However, in the end I learnt both some new things with DX11 an ALOT that I should've already known from before this course.
	// Furthermore, the most important thing. I think DX, 3D programming, is fun, for the first time ever! That gives a nice feeling of accomplishment.
	// Rasmus Tilljander

	void Set();
	void Unset();

private:
	ID3D11Device*               mD3DDevice;
	ID3D11DeviceContext*        mD3DDeviceContext;
	ID3D11ComputeShader*		mShader;
};