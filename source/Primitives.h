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

	struct Circle
	{
		D3DXVECTOR4 MidPosition;
		D3DXVECTOR3	Color;
		float		Radius;
	};

	struct Triangle
	{
		D3DXVECTOR4 Position1;
		D3DXVECTOR4 Position2;
		D3DXVECTOR4 Position3;
		D3DXVECTOR4 Color;
	};

	struct Primitive
	{
		Circle		Circle[2];
		Triangle	Triangle[2];
	};
}

#endif