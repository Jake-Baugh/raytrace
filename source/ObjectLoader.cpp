#include "ObjectLoader.h"

#include <iostream>
#include <fstream>
#include <string>

#include "Primitives.h"
//#include <cstdio>

ObjectLoader* ObjectLoader::m_objectLoader = nullptr;

ObjectLoader* ObjectLoader::GetObjectLoader()
{
	if(m_objectLoader == nullptr)
		m_objectLoader = new ObjectLoader();
	return m_objectLoader;
}

HRESULT ObjectLoader::LoadObject(ID3D11DeviceContext* p_deviceContext, char* p_objPath, char* p_shaderPath, std::vector<CustomPrimitiveStruct::TriangleStruct>** p_out_vertices, std::vector<int>** p_out_indices)

{
	HRESULT hr = S_OK;

	using namespace std;	

	// Vertex worldposition variables
	vector<XMFLOAT4> lVertexPosition;
	lVertexPosition = vector<XMFLOAT4>();
	float lX, lY, lZ;

	// Vertex normal variables
	vector<XMFLOAT3> lVertexNormal;
	lVertexNormal = vector<XMFLOAT3>();
	float lNormalX, lNormalY, lNormalZ;

	// Vertex texture variables
	vector<XMFLOAT2> lTextureCoord;
	lTextureCoord = vector<XMFLOAT2>();
	float uvx, uvy;

	//Face variables
	vector<XMFLOAT3> lFaceDesc;
	lFaceDesc = vector<XMFLOAT3>();

	float x_1, x_2, x_3,
		  y_1, y_2, y_3,
		  z_1, z_2, z_3;

	ifstream lStream;
	lStream.open("Objects/bth.obj");

	char lBuffer[1024];

	while(lStream.getline(lBuffer,1024))
	{
		char lKey[20];

		//Vertex Positions
		//fscanf_s(
		sscanf_s(lBuffer, "%s ", lKey);
		if (lKey[0] == 'v' && lKey[1] != 't' && lKey[1] != 'n')
		{
			sscanf_s(lBuffer,"v %f %f %f", &lX, &lY, &lZ);
			XMFLOAT4 lVector;
			lVector.x = lX;
			lVector.y = lY;
			lVector.z = lZ;
			lVector.w = 1;
			lVertexPosition.push_back(lVector);
		}

		//Vertex Normals
		sscanf_s(lBuffer, "%s ", lKey);
		if (lKey[0] == 'v' && lKey[1] == 'n') 
		{
			sscanf_s(lBuffer,"vn %f %f %f", &lNormalX, &lNormalY, &lNormalZ);
			XMFLOAT3 lVector;
			lVector.x = lNormalX;
			lVector.y = lNormalY;
			lVector.z = lNormalZ;
			lVertexNormal.push_back(lVector);
		}

		//Texture Coordinates
		sscanf_s(lBuffer, "%s ", lKey);
		if (lKey[0] == 'v' && lKey[1] == 't') 
		{
			sscanf_s(lBuffer,"vt %f %f", &uvx, &uvy);
			XMFLOAT2 lVector;
			lVector.x = uvx;
			lVector.y = uvy;
			lTextureCoord.push_back(lVector);
		}

		// Triangle
		sscanf_s(lBuffer, "%s ", lKey);
		if (lKey[0] == 'f')	
		{
			sscanf_s(lBuffer, "f %f/%f/%f %f/%f/%f %f/%f/%f", &x_1, &y_1, &z_1, &x_2, &y_2, &z_2, &x_3, &y_3, &z_3);

			XMFLOAT3 lVector;
			lVector.x = x_1-1;
			lVector.y = y_1-1;
			lVector.z = z_1-1;
			lFaceDesc.push_back(lVector);

			lVector.x = x_2-1;
			lVector.y = y_2-1;
			lVector.z = z_2-1;
			lFaceDesc.push_back(lVector);	

			lVector.x = x_3-1;
			lVector.y = y_3-1;
			lVector.z = z_3-1;
			lFaceDesc.push_back(lVector);			
		}
	}
	
	lStream.close();
	
	
	//Vertices
	vector<CustomPrimitiveStruct::TriangleStruct2> lVertex;
	lVertex = vector<CustomPrimitiveStruct::TriangleStruct2>();
	lVertex.resize(lFaceDesc.size());


	for(int i = 0; i < lFaceDesc.size(); i++)
	{
		lVertex[i].Point1.Position	= (int)lFaceDesc[i].x;
	//	lVertex[i].mNormal			= lVertexNormal[	lFaceDesc[i].y];
		lVertex[i].mTextureCoord	= (int)lFaceDesc[i].z;
	}

	// This is going back

	// A list of all vertices, no copies. ONLY POSITIONS. (lVertexPosition)
	// A list of how to use theese vertices to build triangles. A list of indices. This list should also contain texture coordinate to respective triangle, and maybe normals.



	return hr;
}

struct TriangleDescription
{
	int Vertex0Index;
	int Vertex1Index;
	int Vertex2Index;

};