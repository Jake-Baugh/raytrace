#include "ObjectLoader.h"

ObjectLoader* ObjectLoader::m_objectLoader = nullptr;

ObjectLoader* ObjectLoader::GetObjectLoader()
{
	if(m_objectLoader == nullptr)
		m_objectLoader = new ObjectLoader();
	return m_objectLoader;
}


Object* ObjectLoader::LoadObject(ID3D10Device* lDevice, char* lObjFileName, char* lFXFileName)
{
	using namespace std;	

	//Vertex position variables
	vector<D3DXVECTOR4> lVertexPosition;
	lVertexPosition = vector<D3DXVECTOR4>();
	float lX, lY, lZ;

	//Vertex normal variables
	vector<D3DXVECTOR3> lVertexNormal;
	lVertexNormal = vector<D3DXVECTOR3>();
	float lNormalX, lNormalY, lNormalZ;

	//Vertex texture variables
	vector<D3DXVECTOR2> lTextureCoord;
	lTextureCoord = vector<D3DXVECTOR2>();
	float uvx, uvy;

	//Face variables
	vector<D3DXVECTOR3> lFaceDesc;
	lFaceDesc = vector<D3DXVECTOR3>();

	float x_1,x_2,x_3,y_1,y_2,y_3,z_1,z_2,z_3;

	ifstream lStream;
	lStream.open("Objects/bth.obj");

	char lBuffer[1024];

	while(lStream.getline(lBuffer,1024))
	{
		char lKey[20];

		//Vertex Positions
		sscanf(lBuffer, "%s ", lKey);
		if (lKey[0] == 'v' && lKey[1] != 't' && lKey[1] != 'n')
		{
			sscanf(lBuffer,"v %f %f %f", &lX, &lY, &lZ);
			D3DXVECTOR4 lVector;
			lVector.x = lX;
			lVector.y = lY;
			lVector.z = lZ;
			lVector.w = 1;
			lVertexPosition.push_back(lVector);
		}

		//Vertex Normals
		sscanf(lBuffer, "%s ", lKey);
		if (lKey[0] == 'v' && lKey[1] == 'n') 
		{
			sscanf(lBuffer,"vn %f %f %f", &lNormalX, &lNormalY, &lNormalZ);
			D3DXVECTOR3 lVector;
			lVector.x = lNormalX;
			lVector.y = lNormalY;
			lVector.z = lNormalZ;
			lVertexNormal.push_back(lVector);
		}

		//Texture Coordinates
		sscanf(lBuffer, "%s ", lKey);
		if (lKey[0] == 'v' && lKey[1] == 't') 
		{
			sscanf(lBuffer,"vt %f %f", &uvx, &uvy);
			D3DXVECTOR2 lVector;
			lVector.x = uvx;
			lVector.y = uvy;
			lTextureCoord.push_back(lVector);
		}

		//Faces
		sscanf(lBuffer, "%s ", lKey);
		if (lKey[0] == 'f')	
		{
			sscanf(lBuffer, "f %f/%f/%f %f/%f/%f %f/%f/%f", &x_1, &y_1, &z_1, &x_2, &y_2, &z_2, &x_3, &y_3, &z_3);

				D3DXVECTOR3 lVector;
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
	
	//Vertices
	vector<ObjectVertex> lVertex;
	lVertex = vector<ObjectVertex>();
	lVertex.resize(lFaceDesc.size());


	for(int i = 0; i < lFaceDesc.size(); i++)
	{
		lVertex[i].mPosition		 = lVertexPosition[	lFaceDesc[i].x];
		lVertex[i].mNormal			 = lVertexNormal[	lFaceDesc[i].y];
		lVertex[i].mTextureCoord	 = lTextureCoord[	lFaceDesc[i].z];
	}
	
	lStream.close();

	//Set Vertex Buffer

	ID3D10Buffer* lVBuffer = 0;

	// Create the buffer to kick-off the particle system.
	D3D10_BUFFER_DESC vbd;
    vbd.Usage = D3D10_USAGE_DEFAULT;
	vbd.ByteWidth = sizeof(ObjectVertex) * lVertex.size();
    vbd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    D3D10_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = lVertex.data();
	lDevice->CreateBuffer(&vbd, &vinitData, &lVBuffer);

	Object* lObject = new Object();
	lObject->Initialize(lDevice, lVBuffer, lFXFileName, lVertex.size());

	return lObject;
}