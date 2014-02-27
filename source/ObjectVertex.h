#ifndef OBJECTVERTEX_H
#define OBJECTVERTEX_H

#include "stdafx.h"


struct ObjectVertex
{
	D3DXVECTOR4 mPosition;
	D3DXVECTOR3 mNormal;
	D3DXVECTOR2 mTextureCoord;
};

const D3D10_INPUT_ELEMENT_DESC ObjectVertexDescription[] =
{
	{"POSITION",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0,  0, D3D10_INPUT_PER_VERTEX_DATA, 0},
	{"NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 16, D3D10_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD",    0, DXGI_FORMAT_R32G32_FLOAT,		0, 28, D3D10_INPUT_PER_VERTEX_DATA, 0},
};

const int ObjectVertexInputLayoutNumElements = sizeof(ObjectVertexDescription)/sizeof(ObjectVertexDescription[0]);

#endif