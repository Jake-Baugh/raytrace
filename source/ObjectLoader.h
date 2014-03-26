#ifndef OBJECTLOADER_H
#define OBJECTLOADER_H


#include <vector>
#include "stdafx.h"
#include "Primitives.h"

class ObjectLoader
{
public:
	static ObjectLoader* GetObjectLoader();

	HRESULT LoadObject(ID3D11DeviceContext* p_deviceContext, char* p_objPath, 
		std::vector<XMFLOAT4>** p_out_vertices, 
		std::vector<XMFLOAT2>** p_out_texCoords, 
		std::vector<CustomPrimitiveStruct::TriangleDescription>** p_out_indices, 
		std::vector<XMFLOAT3>** p_out_normals);



private:
	ObjectLoader(){}
	virtual ~ObjectLoader(){}
	static ObjectLoader* m_objectLoader;

	// Map + vector saving loaded objects so that a return is faster.
	// However, if above is done. Let this object take care of memory of loaded objects
};

#endif