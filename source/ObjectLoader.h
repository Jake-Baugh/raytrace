#ifndef OBJECTLOADER_H
#define OBJECTLOADER_H

#include "Object.h"
#include "ObjectVertex.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

class ObjectLoader
{
public:
	static ObjectLoader* GetObjectLoader();

	void LoadObject(ID3D11DeviceContext* p_deviceContext, char* p_objPath, char* p_shaderPath); // Old not implemented method

private:
	ObjectLoader(){}
	virtual ~ObjectLoader(){}
	static ObjectLoader* m_objectLoader;

	// Map + vector saving loaded objects so that a return is faster.
	// However, if above is done. Let this object take care of memory of loaded objects
};

#endif