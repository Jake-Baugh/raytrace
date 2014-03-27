// ALL IN PARAMETERS AND OBJECTS
// HARDCODED VARIABLES
#define EPSILON 0.000001
#define SPHERE_COUNT 3
#define TRIANGLE_COUNT 5
#define LIGHT_COUNT 3
// END OF HARDCODED VARIABLES

// Primitive numbers
#define PRIMITIVE_NOTHING 0
#define SPHERE 1
#define TRIANGLE 2

// COLORS
#define WHITE3 float3(1.0f, 1.0f, 1.0f)
#define BLACK3 float3(0.0f, 0.0f, 0.0f)
#define RED3   float3(1.0f, 0.0f, 0.0f)
#define GREEN3 float3(0.0f, 1.0f, 0.0f)
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
#define GREY4 float4(0.2f, 0.2f, 0.2f, 0.2f)
// END OF COLORS

struct Material
{
	float3 ambient;
	float shininess;
	float3 diffuse;
	float isReflective;
	float3 specular;
	float reflective;
	//	float refractive;
//	float isRefractive;
};

// Shininess is larger for surfaces that are smoother and more mirror-like. When this constant is large the specular highlight is small.

struct PointLightData
{
	float4 position;
	float4 color;
};

struct SphereStruct	// 16
{
	float4	midPos;		// 4
	float3	color;		// 3
	float	radius;		// 1
	Material material;	// 8
};


struct TriangleDescription // 20
{
	float Point0;		// 1
	float Point1;		// 1
	float Point2;		// 1
	float padding1;		// 1
	float TexCoord0;	// 1
	float TexCoord1;	// 1
	float TexCoord2;	// 1
	float padding2;		// 1
	float3 normal;		// 3
	float padding3;		// 1
	Material material;	// 8
};

cbuffer EveryFrameBuffer : register(c0) // 40
{
	float4	 cameraPosition;		// 4
	float4x4 inverseProjection;		// 16
	float4x4 inverseView;			// 16
	//	float4 screenVariable;
}

cbuffer PrimitiveBuffer: register(c1)	// 48 floats, 192 bytes
{
	SphereStruct	Sphere[SPHERE_COUNT];	// 16*3 = 48
	//	float4			countVariable;			// 4
}

cbuffer LightBuffer : register(c2)			// 
{
	float3 ambientLight;					// 3
	float light_count;						// 1
	float3 diffuseLight;
	float PADDING1;
	float3 specularLight;
	float PADDING2;
	PointLightData PointLight[LIGHT_COUNT];	// 
}

RWTexture2D<float4> output								: register(u0);
StructuredBuffer<float4> AllVertex						: register(t0);
StructuredBuffer<float2> AllTexCoord					: register(t1);
StructuredBuffer<TriangleDescription> AllTriangleDesc	: register(t2);
StructuredBuffer<float3> AllNormal						: register(t3);
// END OF INPARAMETERS