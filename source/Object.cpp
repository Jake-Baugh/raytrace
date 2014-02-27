#include "Object.h"

Object::Object()
	:mVertexBuffer(0)
{
//	mShaderObject = new ShaderObject();

//	mPosition  = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 1.0f);
}

Object::~Object()
{
//	SAFE_RELEASE(mVertexBuffer);
//	SAFE_DELETE(mShaderObject);
}

void Object::Initialize(ID3D10Device* lDevice, ID3D10Buffer* lVertexBuffer, char* lFXFileName, int lNumberOfVertices)
{
	/*
	mDevice = lDevice;
	mVertexBuffer = lVertexBuffer;
	mShaderObject->Initialize(mDevice, lFXFileName, D3D10_SHADER_ENABLE_STRICTNESS);
	mShaderObject->AddTechniqueByName(ObjectVertexDescription, ObjectVertexInputLayoutNumElements, "ColorTech");
	mShaderObject->AddTechniqueByName(ObjectVertexDescription, ObjectVertexInputLayoutNumElements, "ColorTechWireFrame");
	mShaderObject->SetResource("Texture", GetResourceLoader().GetBTHTexture());

	D3DXMatrixIdentity(&mWorldMatrix);

	mNumberOfVertices = lNumberOfVertices;

		mWorldMatrix = D3DXMATRIX(
		0.3f,0,0,0,
		0,0.3f,0,0,
		0,0,1,0,
		0,30,0,1
	);

	D3DXMATRIX	lWorldMatrix = D3DXMATRIX(
		1,0,0,0,
		0,1,0,0,
		0,0,0.3f,0,
		0,0,0,1
	);
	D3DXMatrixMultiply(&mWorldMatrix, &mWorldMatrix, &lWorldMatrix);
	*/
}

void Object::Update(float lDeltaTime)
{


}

void Object::Draw()
{
	/*
	mShaderObject->SetMatrix("WorldMatrix", mWorldMatrix);
	mShaderObject->SetMatrix("ViewMatrix", GetCamera().GetViewMatrix());
	mShaderObject->SetMatrix("ProjectionMatrix", GetCamera().GetProjectionMatrix());

	mDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	UINT stride = sizeof(ObjectVertex);
	UINT offset = 0;

	mDevice->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);

	D3D10_TECHNIQUE_DESC techDesc;
	mShaderObject->SetTechniqueByInteger(0);
	mShaderObject->GetTechnique()->GetDesc( &techDesc );
    for(UINT p = 0; p < techDesc.Passes; ++p)
    {
		mShaderObject->Render(p);
		mDevice->Draw(mNumberOfVertices, 0);
    }
	*/
}


