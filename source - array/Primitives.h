#ifndef PRIMITIVES_H
#define PRIMITIVES_H


namespace CustomStruct
{
	struct EachFrameDataStructure
	{
		D3DXMATRIX	inverseProjection;
		D3DXMATRIX	inverseView;
		D3DXVECTOR4 cameraPosition;
		D3DXVECTOR4 screenVariable; // x = screenWidth; y = screenHeight;
		/*
		float screenWidth;
		float screenHeight;
		float padding1;
		float padding2;
		*/
	};

	struct CircleStruct
	{
		D3DXVECTOR4 MidPosition;
		//float		Radius;
	//	D3DXVECTOR4	Color;
	};

	/*
	struct TriangleStruct
	{
		D3DXVECTOR4 Position1;
		D3DXVECTOR4 Position2;
		D3DXVECTOR4 Position3;
		D3DXVECTOR4 Color;
	};*/

	/*
	struct PrimitiveBuffer
	{
		CircleStruct		Circle[1];
	//	TriangleStruct		Triangle[2];
	};
	*/
	
	struct Primitive
	{
		CircleStruct	Circle;
	//	TriangleStruct	Triangle;
	};
	
}

#endif