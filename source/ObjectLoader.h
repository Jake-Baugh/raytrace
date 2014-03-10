#ifndef OBJECTLOADER_H
#define OBJECTLOADER_H


#include <vector>
#include "stdafx.h"
#include "Primitives.h"

class ObjectLoader
{
public:
	static ObjectLoader* GetObjectLoader();

	HRESULT LoadObject(ID3D11DeviceContext* p_deviceContext, char* p_objPath, char* p_shaderPath, std::vector<CustomPrimitiveStruct::TriangleStruct>** p_out_vertices, std::vector<int>** p_out_indices);

private:
	ObjectLoader(){}
	virtual ~ObjectLoader(){}
	static ObjectLoader* m_objectLoader;

	// Map + vector saving loaded objects so that a return is faster.
	// However, if above is done. Let this object take care of memory of loaded objects
};

#endif