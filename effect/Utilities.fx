
// ALL IN PARAMETERS AND OBJECTS
// HARDCODED VARIABLES
#define EPSILON 0.000001
#define SPHERE_COUNT 3
#define LIGHT_COUNT 3
// END OF HARDCODED VARIABLES

// Primitive numbers
#define PRIMITIVE_NOTHING 0
#define PRIMITIVE_SPHERE 1
#define PRIMITIVE_TRIANGLE 2

// COLORS
#define WHITE3 float3(1.0f, 1.0f, 1.0f)
#define BLACK3 float3(0.0f, 0.0f, 1.0f)
#define RED3   float3(1.0f, 0.0f, 1.0f)
#define GREEN3 float3(0.0f, 1.0f, 1.0f)
#define BLUE3  float3(0.0f, 0.0f, 1.0f)
#define TEAL3  float3(0.0f, 1.0f, 1.0f)
#define GREY3  float3(0.2f, 0.2f, 0.2f)

#define WHITE4 float4(1.0f, 1.0f, 1.0f, 1.0f)
#define BLACK4 float4(0.0f, 0.0f, 0.0f, 1.0f)
#define RED4   float4(1.0f, 0.0f, 0.0f, 1.0f)
#define GREEN4 float4(0.0f, 1.0f, 0.0f, 1.0f)
#define BLUE4  float4(0.0f, 0.0f, 1.0f, 1.0f)
#define ORANGE4  float4(1.0f, 0.55f, 0.0f, 1.0f)
#define TEAL4  float4(0.0f, 1.0f, 1.0f, 1.0f)
#define GREY4 float4(0.1f, 0.1f, 0.1f, 1.0f)
// END OF COLORS

#pragma pack_matrix(row_major)
struct Material // 12
{
	float3 ambient;
	float shininess;		// 4
	float3 diffuse;
	float isReflective;		// 4
	float3 specular;
	float reflectivefactor;		// 4

};

// Shininess is larger for surfaces that are smoother and more mirror-like. When this constant is large the specular highlight is small.

struct PointLightData	// 4
{
	float4 position;	// 4
	float4 color;		// 4
//	float4 ambient;		// 4
//	float4 diffuse;		// 4
//	float3 specular;	// 3
//	float  lightRadius; // 1 // atm hardcoded at 50´000
};

struct SphereStruct	// 20
{
	float4	midPos;		// 4
	float3	color;		// 3
	float	radius;		// 1
	Material material;	// 12
};


struct TriangleDescription // 16
{	
	float Point0;		// 1
	float Point1;		// 1
	float Point2;		// 1
	float normalIndex;	// 1
		
	float TexCoordIndex0;	// 1
	float TexCoordIndex1;	// 1
	float TexCoordIndex2;	// 1
	float padding1;			// 1	

//	float padding3;		// 1
	Material material;	// 12
};

cbuffer EveryFrameBuffer : register(c0) // 36
{
	float4	 cameraPosition;		// 4
	float4x4 inverseProjection;		// 16
	float4x4 inverseView;			// 16
}

cbuffer PrimitiveBuffer: register(c1)
{
	SphereStruct	Sphere[SPHERE_COUNT];
}

cbuffer LightBuffer : register(c2)
{
//	float4 ambientLight;					// 3
	PointLightData PointLight[LIGHT_COUNT];	// 8 * 3
}

cbuffer OnePerDispatch: register(c3)
{
	int x_dispatch_count;
	int y_dispatch_count;
	float client_width;
	float client_height;
}

/*
cbuffer GPU_PICK_DATA: register(c4)
{
	int GPU_PICK_X;
	int GPU_PICK_Y;
	int GPU_PICK_padding1;
	int GPU_PICK_padding2;
}
*/


/*
b	Constant buffer
t	Texture and texture buffer
c	Buffer offset
s	Sampler
u	Unordered Access View
http://msdn.microsoft.com/en-us/library/windows/desktop/dd607359(v=vs.85).aspx
*/

RWTexture2D<float4> output								: register(u0);
RWStructuredBuffer<float4> temp							: register(u1);

StructuredBuffer<float4> AllVertex						: register(t0);
StructuredBuffer<TriangleDescription> AllTriangleDesc	: register(t1);
StructuredBuffer<float3> AllNormal						: register(t2);
StructuredBuffer<float2> AllTexCoord					: register(t3);
Texture2D BoxTexture									: register(t4);

SamplerState MeshTextureSampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};


//groupshared int TRIANGLE_INDEX_SELECTED = 0;
