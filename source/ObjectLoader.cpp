#include "ObjectLoader.h"

#include <iostream>
#include <fstream>
#include <string>

#include "Primitives.h"

ObjectLoader* ObjectLoader::m_objectLoader = nullptr;

ObjectLoader* ObjectLoader::GetObjectLoader()
{
	if(m_objectLoader == nullptr)
		m_objectLoader = new ObjectLoader();
	return m_objectLoader;
}

HRESULT ObjectLoader::LoadObject(ID3D11DeviceContext* p_deviceContext, char* p_objPath, std::vector<XMFLOAT4>** p_out_vertices, std::vector<XMFLOAT2>** p_out_texCoords, std::vector<CustomPrimitiveStruct::TriangleDescription>** p_out_indices)

{
	HRESULT hr = S_OK; // THIS VALUE IS ALWAYS S_OK

	using namespace std;	

	// Vertex worldposition variables
	vector<XMFLOAT4>* lVertexPosition;
	lVertexPosition = new vector<XMFLOAT4>;
	float lX, lY, lZ;

	// Vertex normal variables
	vector<XMFLOAT3> lVertexNormal;
	lVertexNormal = vector<XMFLOAT3>();
	float lNormalX, lNormalY, lNormalZ;

	// Vertex texture variables
	vector<XMFLOAT2>* lTextureCoord;
	lTextureCoord = new vector<XMFLOAT2>;

	float uvx, uvy;

	// Mesh description	
	vector<CustomPrimitiveStruct::TriangleDescription>* l_meshDescription;
	l_meshDescription = new vector<CustomPrimitiveStruct::TriangleDescription>;

	int		point_index1, point_index2, point_index3,
			normal_index1, normal_index2, normal_index3,
			texCoord_index1, texCoord_index2, texCoord_index3;

	ifstream lStream;
	//try
	//{
	lStream.open("CUBE2.obj");
	/*}
	catch(exception e)
	{
		hr = S_FALSE;
		return hr;
	}*/

	char lBuffer[1024];

	while(lStream.getline(lBuffer,1024))
	{
		char lKey[20];

		// Texture
		sscanf_s(lBuffer, "%s ", lKey, sizeof(lKey));
		if (lKey[0] == 't' && lKey[1] == 'e' && lKey[2] == 'x')
		{
			sscanf_s(lBuffer,"v %f %f %f", &lX, &lY, &lZ);
			XMFLOAT4 lVector;
			lVector.x = lX;
			lVector.y = lY;
			lVector.z = lZ;
			lVector.w = 1;
			// DO SOMETHING MOAR HERE
		}

		// Vertex Positions
		sscanf_s(lBuffer, "%s ", lKey, sizeof(lKey));
		if (lKey[0] == 'v' && lKey[1] != 't' && lKey[1] != 'n')
		{
			sscanf_s(lBuffer,"v %f %f %f", &lX, &lY, &lZ);
			XMFLOAT4 lVector;
			lVector.x = lX;
			lVector.y = lY;
			lVector.z = lZ;
			lVector.w = 1;
			lVertexPosition->push_back(lVector);
		}

		// Vertex Normals
		sscanf_s(lBuffer, "%s ", lKey, sizeof(lKey));
		if (lKey[0] == 'v' && lKey[1] == 'n') 
		{
			sscanf_s(lBuffer,"vn %f %f %f", &lNormalX, &lNormalY, &lNormalZ);
			XMFLOAT3 lVector;
			lVector.x = lNormalX;
			lVector.y = lNormalY;
			lVector.z = lNormalZ;
			lVertexNormal.push_back(lVector);
		}

		// Texture Coordinates
		sscanf_s(lBuffer, "%s ", lKey, sizeof(lKey));
		if (lKey[0] == 'v' && lKey[1] == 't') 
		{
			sscanf_s(lBuffer,"vt %f %f", &uvx, &uvy);
			XMFLOAT2 lVector;
			lVector.x = uvx;
			lVector.y = uvy;
			lTextureCoord->push_back(lVector);
		}

		// Triangle
		sscanf_s(lBuffer, "%s ", lKey, sizeof(lKey));
		if (lKey[0] == 'f')	
		{
			sscanf_s(lBuffer, "f %i/%i/%i %i/%i/%i %i/%i/%i", 
				&point_index1, &texCoord_index1, &normal_index1, 
				&point_index2, &texCoord_index2, &normal_index2,
				&point_index3, &texCoord_index3, &normal_index3);

			CustomPrimitiveStruct::TriangleDescription l_triangleDesc;
			
			l_triangleDesc.Point1 = point_index1-1;
			l_triangleDesc.Point2 = point_index2-1;
			l_triangleDesc.Point3 = point_index3-1;
			l_triangleDesc.TexCoord1 = texCoord_index1-1;
			l_triangleDesc.TexCoord2 = texCoord_index2-1;
			l_triangleDesc.TexCoord3 = texCoord_index3-1;
			
			l_triangleDesc.Material.ambient = 0.5f;
			l_triangleDesc.Material.diffuse = 0.8f;
			l_triangleDesc.Material.specular = 0.8f;
			l_triangleDesc.Material.shininess = 30.0f;
			l_triangleDesc.Material.reflectiveFactor = 1.0f;
			l_triangleDesc.Material.refractiveFactor = 0.0f;
			l_triangleDesc.Material.isReflective = 1;
			l_triangleDesc.Material.isRefractive = -1;

			// TODO remove hardcoded
			l_meshDescription->push_back(l_triangleDesc);
		}
	}
	
	lStream.close();

	*p_out_vertices = lVertexPosition;
	*p_out_texCoords = lTextureCoord;
	*p_out_indices = l_meshDescription;


	return hr;
}