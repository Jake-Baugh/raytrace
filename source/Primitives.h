#ifndef PRIMITIVES_H
#define PRIMITIVES_H

#include "stdafx.h"

#define SPHERE_COUNT 3
#define TRIANGLE_COUNT 5

namespace CustomPrimitiveStruct
{
	using namespace DirectX;

	struct EachFrameDataStructure
	{
		XMFLOAT4	cameraPosition;
		XMFLOAT4X4	inverseProjection;
		XMFLOAT4X4	inverseView;
	};

	struct Material
	{
		XMFLOAT3 ambient;
		float shininess;
		XMFLOAT3 diffuse;
		float isReflective;
		XMFLOAT3 specular;
		float reflectiveFactor;
	};

	struct SphereStruct
	{
		XMFLOAT4	MidPosition;
		XMFLOAT3	Color;
		float		Radius;
		Material	Material;
	};
	
	struct TriangleDescription // Used for loadedobjects triangles
	{
		float Point1;
		float Point2;
		float Point3;
		float NormalIndex;
		float TexCoord1;
		float TexCoord2;
		float TexCoord3;
		float PADDING1;

		Material Material;
	};

	struct Primitive
	{
		SphereStruct		Sphere[SPHERE_COUNT];
//		float SphereCount;

	};
}

#endif