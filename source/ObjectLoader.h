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

	Object* LoadObject(ID3D10Device* lDevice, char* lObjFileName, char* lFXFileName);

private:
	static ObjectLoader* m_objectLoader;
	
	ObjectLoader();
	virtual ~ObjectLoader();
};

ObjectLoader& GetObjectLoader();


#endif