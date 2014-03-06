#ifndef OBJECT_H
#define OBJECT_H

#include "Camera.h"
#include "ObjectVertex.h"
#include "stdafx.h"

class Object
{
public:
	Object();
	virtual ~Object();
	virtual void Object::Initialize(ID3D10Device* lDevice, ID3D10Buffer* lVertexBuffer, char* lFXFileName, int lNumberOfVertices);

	virtual void Update(float lDeltaTime);
	virtual void Draw();

protected:
	XMFLOAT4 mPosition;
	XMMATRIX mWorldMatrix;
	
	/*
	ID3D10Device* mDevice;
	ShaderObject* mShaderObject;
	*/
	ID3D10Buffer* mVertexBuffer;
	int mNumberOfVertices;

};

#endif
