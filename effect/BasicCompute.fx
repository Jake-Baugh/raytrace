//--------------------------------------------------------------------------------------
// BasicCompute.fx
// Direct3D 11 Shader Model 5.0 Demo
// Copyright (c) Stefan Petersson, 2012
//--------------------------------------------------------------------------------------

#include "LightHelper.fx"
#define EPSILON 0.000001
#define SPHERE_COUNT 3
#define TRIANGLE_COUNT 5
#define LIGHT_COUNT 3


#define WHITE float3(1.0f, 1.0f, 1.0f)
#define BLACK float3(0.0f, 0.0f, 0.0f)
#define RED   float3(1.0f, 0.0f, 0.0f)
#define GREEN float3(0.0f, 1.0f, 0.0f)
#define BLUE  float3(0.0f, 0.0f, 1.0f)

#define WHITE4 float4(1.0f, 1.0f, 1.0f, 1.0f)
#define BLACK4 float4(0.0f, 0.0f, 0.0f, 1.0f)
#define RED4   float4(1.0f, 0.0f, 0.0f, 1.0f)
#define GREEN4 float4(0.0f, 1.0f, 0.0f, 1.0f)
#define BLUE4  float4(0.0f, 0.0f, 1.0f, 1.0f)
#define ORANGE4  float4(1.0f, 0.55f, 0.0f, 1.0f)
#define TEAL4  float4(0.0f, 1.0f, 1.0f, 1.0f)

#pragma pack_matrix(row_major)

RWTexture2D<float4> output : register(u0);

struct SphereStruct
{
	float4	midPos;
	float3	color;
	float	radius;
	Material material;
};

struct TriangleStruct
{	
	float4  pos0;
	float4  pos1;
	float4  pos2;
	float4  color;			//float4 for padding reasons 
	Material material;
};

cbuffer EveryFrameBuffer : register(c0) 
{
	float4	 cameraPosition;
	float4x4 inverseProjection;
	float4x4 inverseView;
	float4 screenVariable;
}

cbuffer PrimitiveBuffer: register(c1)
{
	SphereStruct	Sphere[SPHERE_COUNT];
	TriangleStruct	Triangle[TRIANGLE_COUNT];
	float4			countVariable;
}

cbuffer LightBuffer : register(c2) 
{
	float light_count;
	float3 ambientLight;
	PointLightData PointLight[LIGHT_COUNT];
}

Ray createRay(int x, int y)
{
	Ray l_ray;
	l_ray.origin = cameraPosition;
 
	double normalized_x = ((x / screenVariable.x) - 0.5) * 2;
	double normalized_y = (1 - (y / screenVariable.y) - 0.5) * 2;

	float4 imagePoint = mul(float4(normalized_x, normalized_y, 1, 1), inverseProjection);
	imagePoint /= imagePoint.w;
 
	imagePoint = mul(imagePoint, inverseView);
 
	l_ray.direction = imagePoint - l_ray.origin;
	l_ray.direction = normalize(l_ray.direction);

	return l_ray;
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

interface IntersectInterface
{
	float Intersect(in Ray p_ray, in int p_index);
};

class SphereIntersect : IntersectInterface
{
	float Intersect(in Ray p_ray, in int index)
	{		
		//return 0.0f; //Skip spheres
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
};

class TriangleIntersect : IntersectInterface
{
	float Intersect(Ray p_ray, int index)                         
	{
		//return 0.0f;
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
			return 0.0f;
	
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
};

void GetClosestPrimitive(in Ray p_ray, in IntersectInterface p_intersect, in int p_amount, out int p_hitPrimitive, out int p_closestPrimitiveIndex, out float p_distanceToClosestPrimitive)
{	
	p_hitPrimitive = -1;
	
	int l_primitiveIndex = 0;
	
	float temp = 0.0f;
	float lowest = 0.0f;

	for(int i = 0; i < p_amount; i++)				// Go through all primitives
	{
		temp = p_intersect.Intersect(p_ray, i);				// Get distance to current primitive, return 0.0 if it does not intersect anything
		if(temp != 0.0f && temp > 0.0f)						// if temp has his something and it's bigger than 0 (removing distances that reports negative values)
		{
			p_hitPrimitive = 1;								// if you came here, you have hit something. Now lets check if it's the closest one
			if(temp < lowest || lowest == 0.0f)				// if the new value is lower than the currently lowest OR if the currently lowest is already 0.0, then this new value is the closest
			{
				lowest = temp;								// Save the new lowest distance
				l_primitiveIndex = i;						// Save the index to the lowest primitive
			}
		}
	}
	p_closestPrimitiveIndex = l_primitiveIndex;				// Return values
	p_distanceToClosestPrimitive = lowest;
}

// Make this function return true or false. Let other functions handle the coloring. Also then remove color and material from parameterlist
bool IsLitByLight(in Ray p_ray, in int p_primitiveIndex, in bool p_isTriangle, in int p_lightIndex)
{
	float4 l_tempColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
	SphereIntersect sphereIntersect;
	TriangleIntersect triangleIntersect;
	
	// Vector from light source
	Ray l_lightSourceRay;
	l_lightSourceRay.origin = PointLight[p_lightIndex].position;
	l_lightSourceRay.direction = normalize(p_ray.origin - PointLight[p_lightIndex].position);
	
	int l_closestSphereIndex, l_closestTriangleIndex;
	float l_distanceToClosestSphere, l_distanceToClosestTriangle = 0.0f;
	int l_sphereHit = -1;
	int l_TriangleHit = -1;
		
	GetClosestPrimitive(l_lightSourceRay, sphereIntersect, countVariable.x, l_sphereHit, l_closestSphereIndex, l_distanceToClosestSphere);
	GetClosestPrimitive(l_lightSourceRay, triangleIntersect, countVariable.y, l_TriangleHit, l_closestTriangleIndex, l_distanceToClosestTriangle);
		
	if(l_sphereHit != -1 && l_TriangleHit != -1) // Both a triangle and a sphere has been hit
	{
		if(p_isTriangle == true) // Bouncing of a triangle
		{
			if(l_distanceToClosestTriangle < l_distanceToClosestSphere) // Triangle is closest
			{
				if(p_primitiveIndex == l_closestTriangleIndex)	// The triangle that I bounced of is the closest
				{
					return true; // Render is lit
				}
			}
		}
		else if(p_isTriangle == false) // Input primitive is NOT triangle. Thus sphere
		{
			if(l_distanceToClosestSphere < l_distanceToClosestTriangle) // Sphere is the closest
			{
				if(p_primitiveIndex == l_closestSphereIndex)  
				{
					return true; // Sphere is lit
				}
			}
		}
	}
	else if(l_sphereHit != -1 && l_TriangleHit == -1) // Only a sphere was hit
	{
		if(p_primitiveIndex == l_closestSphereIndex)	// The sphere I am at is the closest
		{		
			return true; // Sphere is lit
		}
	}
	else if(l_sphereHit == -1 && l_TriangleHit != -1) // Only a triangle was hit
	{
		if(p_primitiveIndex == l_closestTriangleIndex)	// The triangle I am at is the closest
		{
			return true; // Triangle is lit
		}
	}
	return false;
}

	// returns next ray // does not return a color
Ray Jump(in Ray p_ray, out float4 p_out_collideNormal, out Material p_out_material, out int p_out_primitiveIndex, out bool p_out_isTriangle) 
{	
	Ray l_ray = p_ray;

	// Variables used by all intersections
	float4 l_collidePos;

	int l_sphereindex = 0;	
	int l_triangleindex = 0;
	float l_distanceToClosestSphere	= 0.0f;
	float l_distanceToClosestTriangle = 0.0f;


	SphereIntersect sphereIntersect;
	TriangleIntersect triangleIntersect;
	
	int l_sphereHit, l_triangleHit;

	GetClosestPrimitive(p_ray, sphereIntersect, countVariable.x, l_sphereHit, l_sphereindex, l_distanceToClosestSphere); // Sphere
	GetClosestPrimitive(p_ray, triangleIntersect, countVariable.y, l_triangleHit, l_triangleindex, l_distanceToClosestTriangle); // Triangle
	
	// Checks to se if any triangle or sphere was hit at all
	if(l_distanceToClosestTriangle == 0.0f && l_distanceToClosestSphere == 0.0f) 
	{
		/*if(l_sphereindex == 0 || l_triangleindex == 0)
		{
			
			a.origin = float4(123.0f, 0, 0,0);
			return a;
		}*/
		return p_ray;
	}

	////////////////////////////// Checks which primitive is closest

	if((l_sphereHit != -1 && l_triangleHit != -1 && l_distanceToClosestSphere < l_distanceToClosestTriangle) || l_sphereHit != -1 && l_triangleHit == -1)
	{			
		// Reflect code
		l_collidePos = p_ray.origin + (l_distanceToClosestSphere - 0.0001) * p_ray.direction;

		// Out variables
		p_out_collideNormal = -normalize(l_collidePos - Sphere[l_sphereindex].midPos); // Reverse normal
		p_out_material = Sphere[l_sphereindex].material;
		p_out_primitiveIndex = l_sphereindex;
		p_out_isTriangle = false;

		// New variables for next ray
		l_ray.origin = l_collidePos; 
		l_ray.direction = float4(reflect(p_ray.direction.xyz, p_out_collideNormal), 0.0f); // new direction for next jump

	}
	else if((l_sphereHit != -1 && l_triangleHit != -1 && l_distanceToClosestTriangle < l_distanceToClosestSphere) || l_sphereHit == -1 && l_triangleHit != -1)
	{	
		// Reflect code
		l_collidePos = p_ray.origin + (l_distanceToClosestTriangle - 0.0001) * p_ray.direction;
		
		// Out variables		
		p_out_collideNormal = float4(TriangleNormalCounterClockwise(l_triangleindex), 1.0f);
		p_out_material = Triangle[l_triangleindex].material;
		p_out_primitiveIndex = l_triangleindex;
		p_out_isTriangle = true;

		// New variables for next ray
		l_ray.origin = l_collidePos;
		l_ray.direction = float4(reflect(p_ray.direction.xyz, p_out_collideNormal), 0.0f);		
	}	
	else // This is a debug place, should never happen.
	{
		// Mighty guy
	}

	return l_ray;
}

float4 ThrowRefractionRays(in Ray p_ray, in float4 p_collidNormal)
{
	float n1, n2;
	float angle1, angle2;
	
	/*
	cos(angle) = vec1 * vec2 / |vec1|*|vec2|
	angle = arccos (vec1* vec2)

	n1 = 1.0f // vacuum
	n2 = 1.5f // glass
	angle1 = angle between infallsvektor och normal
	angle2 = angle between normal och utfallsvinkel

	n1 * sin(angle1) = n2 * sin(angle2)
	sin(angle2) = n1 * sin(angle1) / n2
	angle2 = arcsin(n1 * sin(angle1) / n2)
	*/
}


float4 GetPrimitiveColor(in int p_primitiveIndex, in bool p_isTriangle)
{
	if(!p_isTriangle)	// Sphere
		return float4(Sphere[p_primitiveIndex].color, 0.0f);
	else				// Triangle
		return Triangle[p_primitiveIndex].color;	
}

float4 GetReflectiveFactor(in int p_primitiveIndex, in bool p_isTriangle)
{
	if(!p_isTriangle)	// Sphere
		return Sphere[p_primitiveIndex].material.reflective;
	else				// Triangle
		return Triangle[p_primitiveIndex].material.reflective;	
}


float4 Shade(in Ray p_ray, in int p_primitiveIndex, in bool p_isTriangle, in float4 p_collideNormal, in Material p_material)
{
	float4 p_color = float4(0.0f, 0.0f, 0.0f, 0.0f);
	
	float4 l_primitiveColor = GetPrimitiveColor(p_primitiveIndex, p_isTriangle);

	for(int i = 0; i < LIGHT_COUNT; i++) // for each light
	{	
		// Light and shadows
		bool l_isLitByLight = IsLitByLight(p_ray, p_primitiveIndex, p_isTriangle, i);
		if(l_isLitByLight == true) // Thus is lit
			p_color += l_primitiveColor * CalcLight(p_ray, PointLight[i], p_ray.origin, p_collideNormal, p_material, float4(ambientLight, 1.0f));
	}
	return p_color;
}

#define max_number_of_bounces 2
float4 Trace(in Ray p_ray)
{
	Ray l_nextRay = p_ray;
	float4 colorIllumination	= float4(0.0f, 0.0f, 0.0f, 0.0f);

	float4 l_collideNormal;
	Material l_material;
	int l_primitiveIndex;
	bool l_isTriangle;
	float l_reflectiveFactor;
	
	l_nextRay = Jump(p_ray, l_collideNormal, l_material, l_primitiveIndex, l_isTriangle);
	colorIllumination += Shade(l_nextRay, l_primitiveIndex, l_isTriangle, l_collideNormal, l_material);
	
	l_nextRay = Jump(l_nextRay, l_collideNormal, l_material, l_primitiveIndex, l_isTriangle);
	colorIllumination += Shade(l_nextRay, l_primitiveIndex, l_isTriangle, l_collideNormal, l_material);
	/*if(l_nextRay.origin.x == 123.0f)
		colorIllumination = ORANGE4;
	if(l_nextRay.origin.x == 246.0f)
		colorIllumination = TEAL4;*/
//	l_nextRay = Jump(l_nextRay, l_collideNormal, l_material, l_primitiveIndex, l_isTriangle);
//	colorIllumination += Shade(l_nextRay, l_primitiveIndex, l_isTriangle, l_collideNormal, l_material);
		
//	l_nextRay = Jump(p_ray, l_collideNormal, l_material, l_primitiveIndex, l_isTriangle);
//	colorIllumination += Shade(l_nextRay, l_primitiveIndex, l_isTriangle, l_collideNormal, l_material);

//	l_reflectiveFactor = GetReflectiveFactor(l_primitiveIndex, l_isTriangle);
	
//	if(l_reflectiveFactor != 0.0f)
//	{
	/*
		for(int i = 0; i < max_number_of_bounces; i++)
		{
			l_nextRay = Jump(l_nextRay, l_collideNormal, l_material, l_primitiveIndex, l_isTriangle);
			l_reflectiveFactor = GetReflectiveFactor(l_primitiveIndex, l_isTriangle);

			//colorIllumination += Shade(l_nextRay, l_primitiveIndex, l_isTriangle, l_collideNormal, l_material);


			//if(l_reflectiveFactor == 0.0f)
			//	break;
		}*/
//	}
	//float4 l_finalColor = colorIllumination + colorReflected + colorRefracted;
	return colorIllumination;
}

[numthreads(32, 32, 1)]
void main( uint3 threadID : SV_DispatchThreadID)
{
	float4 l_finalColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	Ray l_ray = createRay(threadID.x, threadID.y);
	l_finalColor = Trace(l_ray);

	float a;
	a = max(l_finalColor.x, l_finalColor.y);
	a = max(a, l_finalColor.z);
	a = max(a, 1.0f);
	
	l_finalColor /= a;

	output[threadID.xy] = l_finalColor;
}

/*
	Fixa så att ljuset rör sig.
	Fixa till så att det ser bra ut.
	Väldigt skrikiga färger
	Fixa flimmret som är
	Dela upp saker i funktioner
	Dynamiska buffrar
	Ladda in objekt
	Material
	Ljus som studsar? :3

	Tankar
		Octatree
		Hashtable som använder pekare som nyckel
		Boundingbox för varje objekt för snabbare koll.
		Skapa egna ComputeShaders

*/