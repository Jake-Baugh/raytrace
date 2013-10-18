#ifndef PRIMITIVES_H
#define PRIMITIVES_H


namespace CustomStruct
{
	struct EachFrameDataStructure
	{
		D3DXVECTOR4 cameraPosition;
		D3DXMATRIX	inverseProjection;
		D3DXMATRIX	inverseView;	
		//D3DXVECTOR4 screenVariable;
		float screenWidth;
		float screenHeight;
		float padding1;
		float padding2;
	};

	struct SphereStruct
	{
		D3DXVECTOR4 MidPosition;
		D3DXVECTOR3	Color;
		float		Radius;
	};

	struct TriangleStruct
	{
		D3DXVECTOR4 Position0;
		D3DXVECTOR4 Position1;
		D3DXVECTOR4 Position2;
		D3DXVECTOR4 Color;
	};

	struct Primitive
	{
		SphereStruct		Sphere[2];
		TriangleStruct		Triangle[2];
		float SphereCount;
		float TriangleCount;
		float padding1;
		float padding2;
	};
}

#endif