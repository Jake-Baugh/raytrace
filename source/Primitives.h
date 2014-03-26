#ifndef PRIMITIVES_H
#define PRIMITIVES_H

#define SPHERE_COUNT 3
#define TRIANGLE_COUNT 5

namespace CustomPrimitiveStruct
{
	using namespace DirectX;

	struct EachFrameDataStructure
	{
		XMFLOAT4	cameraPosition;
		XMMATRIX	inverseProjection;
		XMMATRIX	inverseView;	
	//	float screenWidth;
	//	float screenHeight;
	//	float padding1;
	//	float padding2;
	};

	struct Material
	{
		float ambient;
		float specular;
		float diffuse;
		float shininess;		
		float reflectiveFactor;
		float refractiveFactor;
		float isReflective;
		float isRefractive;
	};

	struct SphereStruct
	{
		XMFLOAT4	MidPosition;
		XMFLOAT3	Color;
		float		Radius;
		Material	Material;
	};

	/*
	struct TriangleStruct // Used for hardcoded triangles
	{
		XMFLOAT4	Position0;
		XMFLOAT4	Position1;
		XMFLOAT4	Position2;
		XMFLOAT4	Color;
		Material	Material;
	};
	*/
	
	struct TriangleDescription // Used for loadedobjects triangles
	{
		float Point1;
		float Point2;
		float Point3;
		float PADDING1;
		float TexCoord1;
		float TexCoord2;
		float TexCoord3;
		float PADDING2;
		XMFLOAT3 Normal;
		float PADDING3;
		Material Material;
	};

	struct Primitive
	{
		SphereStruct		Sphere[SPHERE_COUNT];
	//	TriangleStruct		Triangle[TRIANGLE_COUNT];
/*		float SphereCount;
		float TriangleCount;
		float TriangleCountFromObject;
		float padding1;
*/
	};
}

#endif