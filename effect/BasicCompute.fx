//--------------------------------------------------------------------------------------
// BasicCompute.fx
// Direct3D 11 Shader Model 5.0 Demo
// Copyright (c) Stefan Petersson, 2012
//--------------------------------------------------------------------------------------

#include "LightHelper.fx"

#pragma pack_matrix(row_major)

RWTexture2D<float4> output : register(u0);

cbuffer EveryFrameBuffer : register(c0) 
{
	float4	 cameraPosition;
	float4x4 inverseProjection;
	float4x4 inverseView;
	float4 screenVariable;
	//float screenWidth;
	//float screenHeight;
	//float padding1;
	//float padding2;
}

struct SphereStruct
{
	float4	midPos;
	float3	color;
	float	radius;
};


struct TriangleStruct
{	
	float4  pos0;
	float4  pos1;
	float4  pos2;
	float4  color;			//float4 for padding reasons 
};

cbuffer PrimitiveBuffer: register(c1)
{
	SphereStruct	Sphere[2];
	TriangleStruct	Triangle[2];
	float4			countVariable;
}

cbuffer LightBuffer : register(c2) 
{
	LightData Light[1];
}

struct Ray
{
	float4 origin;
	float4 direction;
	float4 color;
	bool hit;
};

Ray createRay(int x, int y)
{
	Ray l_ray;
	l_ray.origin = cameraPosition;
 
	double normalized_x = ((x / screenVariable.x) - 0.5) * 2;
	double normalized_y = (1 - (y / screenVariable.y) - 0.5) * 2;

//	double normalized_x = ((x / screenWidth) - 0.5) * 2;		// Other variable name
//	double normalized_y = (1 - (y / screenHeight) - 0.5) * 2;	// Other variable name
 
	float4 imagePoint = mul(float4(normalized_x, normalized_y, 1, 1), inverseProjection);
	imagePoint /= imagePoint.w;
 
	imagePoint = mul(imagePoint, inverseView);
 
	l_ray.direction = imagePoint - l_ray.origin;
	l_ray.direction = normalize(l_ray.direction);
//	l_ray.direction.z = 0.0f;						// Used to test code
	l_ray.color = float4(0.0f, 0.0f, 0.0f, 1.0f);

	l_ray.hit = false;
	return l_ray;
}

float SphereIntersect(Ray p_ray, int index)
{		
	float4 l_distance = p_ray.origin - Sphere[index].midPos;
	float a, b, t, t1, t2;

	b = dot(p_ray.direction, l_distance);
	a = dot(l_distance, l_distance ) - (Sphere[index].radius * Sphere[index].radius);
	if(b * b - a >= 0)
	{
		t = sqrt(b * b - a);
		t1 = -b + t;  
		t2 = -b - t;
		if( t1 > 0.0f || t2 > 0.0f) // checks so atleast one of the values are infront of the camera
		{			
			if(t1 < t2 && t1 > 0)
				return t1;
			else if( t2 > 0)
				return t2;
			//return t1 < t2 ? t1 : t2;		//Returns the smallest number
		}
	}
	return 0.0f;		// ifwe didn't hit	
}

#define EPSILON 0.000001
float TriangleIntersect(Ray p_ray, int index)                         
{
	//return 1.0f;
	float3 e1, e2;  //Edge1, Edge2
	float det, inv_det, u, v;
	float t;
 
	//Find vectors for two edges sharing V0
	e1 = Triangle[index].pos1.xyz - Triangle[index].pos0.xyz;
	e2 = Triangle[index].pos2.xyz - Triangle[index].pos0.xyz;	
	
	//Begin calculating determinant - also used to calculate u parameter
	float3 P = cross(p_ray.direction.xyz, e2);
	
	//if determinant is near zero, ray lies in plane of triangle
	det = dot(e1, P);
	
	//NOT CULLING
	if(det > -EPSILON && det < EPSILON) 
		return 0;
	
	inv_det = 1.0f / det;

	//calculate distance from V0 to ray origin
	float3 T = p_ray.origin.xyz - Triangle[index].pos0.xyz;

	//Calculate u parameter and test bound
	u = dot(T, P) * inv_det;
	
	//The intersection lies outside of the triangle
	if(u < 0.0f || u > 1.0f) 
		return 0.0f;

	//Prepare to test v parameter
	float3 Q = cross(T, e1);

	//Calculate V parameter and test bound
	v = dot(p_ray.direction.xyz, Q) * inv_det;
	
	//The intersection lies outside of the triangle
	if(v < 0.0f || u + v  > 1.0f) 
		return 0.0f;

	t = dot(e2, Q) * inv_det;

	if(t > EPSILON) 
		return t; //ray intersection

	// No hit, no win
	return 0.0f;
}

float3 TriangleNormalCounterClockwise(int index)
{
	float3 e1, e2;  //Edge1, Edge2
 
	//Find vectors for two edges sharing V0
	e1 = Triangle[index].pos1.xyz - Triangle[index].pos0.xyz;
	e2 = Triangle[index].pos2.xyz - Triangle[index].pos0.xyz;	
	
	float3 normal = cross(e1, e2);
	return normalize(normal);	
}

Ray RayUpdate(Ray p_ray)
{	
	// Variables used by all intersections
	float4 l_tempColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 l_collidePos, l_collideNormal;

	// Sphere specific variable
	float l_spherehit	= 0.0f;
	int l_sphereindex = 0;

	float l_trianglehit = 0.0f;
	int l_triangleindex = 0;


	for(int i = 0; i < countVariable.x; i++)				// Go through all spheres
	{
		float temp = SphereIntersect(p_ray, i);				// Get distance to current sphere, return 0.0 if it does not intersect
		if(temp != 0.0f)									// Checks so that it does intersect
		{
			if(temp < l_spherehit || l_spherehit == 0.0f)	// Sets the new value to l_spherehit if it's lower than before or if l_spherehit equals 0.0f (first attempt)
			{
				l_spherehit = temp;
				l_sphereindex = i;							// Saves index to closes sphere
			}
		}
	}
	
	// Same code as above but for triangles instead
	for(int i = 0; i < countVariable.y; i++)
	{
		float temp = TriangleIntersect(p_ray, i);
		if(temp != 0.0f)
		{
			if(temp < l_trianglehit || l_trianglehit == 0.0f)
			{
				l_trianglehit = temp;
				l_triangleindex = i;
			}
		}
	}

	////////////////////////////// Checks to se if any triangle or sphere was hit at all

	if(l_trianglehit == 0.0f && l_spherehit == 0.0f)
	{
		return p_ray; // It hit nothing, return direct
	}

	////////////////////////////// Checks which primitive is closest
	
	if(0.0f != l_spherehit && (l_trianglehit == 0.0f || l_trianglehit < 0.0f || l_spherehit < l_trianglehit))
	{
		l_tempColor = float4(Sphere[l_sphereindex].color, 1);
		p_ray.hit = true;
		
		// Reflect code
		l_collidePos = p_ray.origin + (l_spherehit - 0.0001) * p_ray.direction;
		l_collideNormal = normalize((Sphere[l_sphereindex].midPos - p_ray.origin));
		
		p_ray.origin = l_collidePos;
		p_ray.direction = reflect(p_ray.direction, l_collideNormal); 
				
		// Make light code here
		SurfaceInfo a;
		a.position = l_collidePos;
		a.normal = l_collideNormal;
		a.diffuse = l_tempColor;
		a.specular = float4(1.0f, 1.0f, 1.0f, 1.0f);
		l_tempColor = PointLight(a, Light[0], cameraPosition);
		//if(l_tempColor.x == 0.0f && l_tempColor.y == 0.0f && l_tempColor.z == 0.0f && l_tempColor.w == 0.0f)
			//l_tempColor = float4(1.0f, 0.5f, 0.5f, 0.0f);
	}
	else if(0.0f != l_trianglehit && 0.0f < l_trianglehit && (l_spherehit == 0.0f || l_trianglehit < l_spherehit))
	{
		
		l_tempColor = Triangle[l_triangleindex].color;
		p_ray.hit = true;

		// Make reflect code here
		l_collidePos = p_ray.origin + (l_trianglehit - 0.0001) * p_ray.direction;
		l_collideNormal = float4(TriangleNormalCounterClockwise(l_triangleindex), 1.0f);
		p_ray.origin = l_collidePos;
		p_ray.direction = reflect(p_ray.direction, l_collideNormal);
	}	
	else
	{
		// This is a debug place, should never happen.
		l_tempColor = float4(0.0f, 0.0f, 1.0f, 1.0f);
		p_ray.hit = true;
	}
	p_ray.color = l_tempColor;
	//p_ray.color += l_tempColor;

	return p_ray;
}

#define max_number_of_bounces 3
[numthreads(32, 32, 1)]
void main( uint3 threadID : SV_DispatchThreadID)
{
	float4 l_finalColor = float4(0,0,0,1);
	Ray l_ray = createRay(threadID.x, threadID.y);
		
	for(int i = 0; i < max_number_of_bounces; i++)	
	{
		l_ray = RayUpdate(l_ray);
	}

	output[threadID.xy] = l_ray.color;
//	output[threadID.xy] = l_ray.direction; //Debug thingy sak för att se om rays faktiskt blir nåt	
}