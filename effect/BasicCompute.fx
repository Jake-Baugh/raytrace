//--------------------------------------------------------------------------------------
// BasicCompute.fx
// Direct3D 11 Shader Model 5.0 Demo
// Copyright (c) Stefan Petersson, 2012
//--------------------------------------------------------------------------------------

#include "LightHelper.fx"

Ray createRay(uint x, uint y)
{
	Ray l_ray;
	l_ray.origin = cameraPosition; 

	double normalized_x = ((x / client_width) - 0.5) * 2.0;					// HARDCODED SCREENSIZE
	double normalized_y = (1 - (y / client_height) - 0.5) * 2.0;				// HARDCODED SCREENSIZE

	float4 imagePoint = mul(float4(normalized_x, normalized_y, 1.0f, 1.0f), inverseProjection);
	imagePoint /= imagePoint.w;
 
	imagePoint = mul(imagePoint, inverseView);
 
	l_ray.direction = imagePoint - l_ray.origin;
	l_ray.direction = normalize(l_ray.direction);

	return l_ray;
}

float3 TriangleNormalCounterClockwise(uint DescriptionIndex)
{
	return normalize(AllNormal[(uint)AllTriangleDesc[DescriptionIndex].normalIndex]);
}

interface IntersectInterface
{
	float Intersect(in Ray p_ray, in uint p_index);
};

class SphereIntersect : IntersectInterface
{
	float Intersect(in Ray p_ray, in uint index)
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
			}
		}
		return 0.0f;		// ifwe didn't hit	
	}
};

class TriangleIntersect : IntersectInterface
{
	float Intersect(in Ray p_ray, in uint index)                         
	{
		float3 e1, e2;
		float det, inv_det, u, v;
		float t;

		float Point0, Point1, Point2;

		uint tempindex = index;

		Point0 = AllTriangleDesc[tempindex].Point0;
		Point1 = AllTriangleDesc[tempindex].Point1;
		Point2 = AllTriangleDesc[tempindex].Point2;
		
		//Find vectors for two edges sharing V0
		e1 = AllVertex[(uint)Point1].xyz - AllVertex[(uint)Point0].xyz;
		e2 = AllVertex[(uint)Point2].xyz - AllVertex[(uint)Point0].xyz;
		
	
		//Begin calculating determinant - also used to calculate u parameter
		float3 P = cross(p_ray.direction.xyz, e2);
	
		//if determinant is near zero, ray lies in plane of triangle
		det = dot(e1, P);
	
		//NOT CULLING
		if(det > -EPSILON && det < EPSILON) 
			return 0.0f;
	
		inv_det = 1.0f / det;

		//calculate distance from V0 to ray origin
		float3 T = p_ray.origin.xyz - AllVertex[Point0].xyz;

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

const SphereIntersect sphereIntersect;
const TriangleIntersect triangleIntersect;

void GetClosestPrimitive(in Ray p_ray, in IntersectInterface p_intersect, in uint p_amount, out uint p_hitPrimitive, out uint p_closestPrimitiveIndex, inout float p_distanceToClosestPrimitive)
{	
	p_hitPrimitive = -1;	
	float temp = 0.0f;
	p_distanceToClosestPrimitive = 0.0f;

	for(uint i = 0; i < p_amount; i++)				// Go through all primitives
	{
		temp = p_intersect.Intersect(p_ray, i);				// Get distance to current primitive, return 0.0 if it does not intersect anything
		if(temp != 0.0f && temp > 0.0f)						// if temp has his something and it's bigger than 0 (removing distances that reports negative values)
		{
			p_hitPrimitive = 1;								// if you came here, you have hit something. Now lets check if it's the closest one
			if (temp < p_distanceToClosestPrimitive || p_distanceToClosestPrimitive == 0.0f)				// if the new value is lower than the currently lowest OR if the currently lowest is already 0.0, then this new value is the closest
			{
				p_distanceToClosestPrimitive = temp;				// Save the new lowest distance
				p_closestPrimitiveIndex = i;						// Save the index to the lowest primitive
			}
		}
	}
}

/*
	Rework this to take distance to light and set origin from object and check if anything is closer than the light.
	Right now I check from lightsource to object to see if anything intersects before the object.
*/
bool IsLitByLight(in Ray p_ray, in uint p_primitiveIndex, in uint p_primitiveType, in uint p_lightIndex)
{
	uint l_closestSphereIndex, l_closestTriangleIndex;
	float l_distanceToClosestSphere, l_distanceToClosestTriangle;
	uint l_sphereHit;
	uint l_TriangleHit;
	
	// Vector from light source
	Ray l_lightSourceRay;
	l_lightSourceRay.origin = PointLight[p_lightIndex].position;
	l_lightSourceRay.direction = normalize(p_ray.origin - PointLight[p_lightIndex].position);

	uint triangle_amount;
	AllTriangleDesc.GetDimensions(triangle_amount, l_closestSphereIndex); // Needed to send something as second paramter!! (?)!

	GetClosestPrimitive(l_lightSourceRay, sphereIntersect, SPHERE_COUNT, l_sphereHit, l_closestSphereIndex, l_distanceToClosestSphere);
	GetClosestPrimitive(l_lightSourceRay, triangleIntersect, triangle_amount, l_TriangleHit, l_closestTriangleIndex, l_distanceToClosestTriangle);
		
	if(l_sphereHit != -1 && l_TriangleHit != -1) // Both a triangle and a sphere has been hit
	{
		if (p_primitiveType == PRIMITIVE_TRIANGLE) // Bouncing of a triangle
		{
			if(l_distanceToClosestTriangle < l_distanceToClosestSphere) // Triangle is closest
			{
				if(p_primitiveIndex == l_closestTriangleIndex)	// The triangle that I bounced of is the closest
				{
					return true; // Triangle is lit
				}
			}
		}
		else if (p_primitiveType == PRIMITIVE_SPHERE) // Input primitive is NOT triangle. Thus sphere
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


#define VERY_SMALL_NUMBER 0.001f
Ray Jump(inout Ray p_ray, out float4 p_out_collideNormal, out Material p_out_material, out uint p_out_primitiveIndex, out uint p_out_primitiveType)
{	
	p_out_collideNormal = float4(0.0f, 0.0f, 0.0f, 0.0f);

	/*
	p_out_material.ambient			= float3(0.0f, 0.0f, 0.0f);
	p_out_material.shininess		= 0.0f;
	p_out_material.isReflective		= 0.0f;
	p_out_material.specular			= float3(0.0f, 0.0f, 0.0f);
	p_out_material.reflectivefactor = float4(0.0f, 0.0f, 0.0f, 0.0f);
	*/

	p_out_primitiveIndex = 0;
	p_out_primitiveType = 0;

	// Variables used by all intersections
	float4 l_collidePos;
		
	uint l_sphereindex = 0;	
	uint l_triangleindex = 0;
	float l_distanceToClosestSphere	= 0.0f;
	float l_distanceToClosestTriangle = 0.0f;	
	uint l_sphereHit, l_triangleHit;

	uint triangle_amount;
	AllTriangleDesc.GetDimensions(triangle_amount, l_sphereindex); // Needed to send something as second paramter!! (?)!

	GetClosestPrimitive(p_ray, sphereIntersect, SPHERE_COUNT, l_sphereHit, l_sphereindex, l_distanceToClosestSphere); // Sphere
	GetClosestPrimitive(p_ray, triangleIntersect, triangle_amount, l_triangleHit, l_triangleindex, l_distanceToClosestTriangle); // Triangle
	
	// Checks to se if any triangle or sphere was hit at all
	if(l_distanceToClosestTriangle == 0.0f && l_distanceToClosestSphere == 0.0f) 
	{
		p_out_primitiveType = PRIMITIVE_NOTHING; // Not a triangle or sphere
		return p_ray;
	}

	////////////////////////////// Checks which primitive is closest
	if((l_sphereHit != -1 && l_triangleHit != -1 && l_distanceToClosestSphere < l_distanceToClosestTriangle) || l_sphereHit != -1 && l_triangleHit == -1)
	{			
		// Reflect code
		l_collidePos = p_ray.origin + (l_distanceToClosestSphere - VERY_SMALL_NUMBER) * p_ray.direction;

		// Out variables
		p_out_collideNormal		= normalize(l_collidePos - Sphere[l_sphereindex].midPos); // Reverse normal
		p_out_material			= Sphere[l_sphereindex].material;
		p_out_primitiveIndex	= l_sphereindex;
		p_out_primitiveType		= PRIMITIVE_SPHERE;

		// New variables for next ray
		p_ray.origin = l_collidePos;
		p_ray.direction = float4(reflect(p_ray.direction.xyz, p_out_collideNormal.xyz), 0.0f); // new direction for next jump
	}
	else if((l_sphereHit != -1 && l_triangleHit != -1 && l_distanceToClosestTriangle < l_distanceToClosestSphere) || l_sphereHit == -1 && l_triangleHit != -1)
	{	
		// Reflect code
		l_collidePos = p_ray.origin + (l_distanceToClosestTriangle - VERY_SMALL_NUMBER) * p_ray.direction;
		
		// Out variables		
		p_out_collideNormal		= float4(TriangleNormalCounterClockwise(l_triangleindex), 0.0f); // Do not normalize this. Already normalized
		p_out_material			= AllTriangleDesc[l_triangleindex].material;
		p_out_primitiveIndex	= l_triangleindex;
		p_out_primitiveType		= PRIMITIVE_TRIANGLE;

		// New variables for next ray
		p_ray.origin			= l_collidePos;
		p_ray.direction			= float4(reflect(p_ray.direction.xyz, -p_out_collideNormal.xyz), 0.0f);
	}	
	else // Hit nothing
	{
		p_out_primitiveType		= PRIMITIVE_NOTHING;
	}

	return p_ray;
}

float GetTriangleArea(float3 point0, float3 point1, float3 point2)
{
	float border0, border1, border2;
	border0 = length(point0 - point1);
	border1 = length(point0 - point2);
	border2 = length(point1 - point2);

	float temp1, temp2;
	if ((temp1 = border0) == (temp2 = border1) || (temp1 = border0) == (temp2 = border2) || (temp1 = border1) == (temp2 = border2)) // if two sides are equally long you can cheat
	{
		return temp1 * temp2 / 2; // Half square area
	}
			
	// Herons Formula // http://en.wikipedia.org/wiki/Heron%27s_formula
	float s = 0.5f * (border0+border1+border2); //  semiperimeter 
	float area = sqrt(s * (s - border0) * (s - border1) * (s - border2));	

	return area;
}

// Barycentric interpolation
float2 GetTriangleTextureCoordinates(in uint p_primitiveIndex, in float3 p_intersectPos)
{
	// Får kolla att två sidor inte är lika långa. Är dem lika långa så kan man använda dem, s1*s1/2, halva kvadratarean

	TriangleDescription l_triangleDescription = AllTriangleDesc[p_primitiveIndex];

	float totalArea = GetTriangleArea(AllVertex[l_triangleDescription.Point0].xyz, AllVertex[l_triangleDescription.Point1].xyz, AllVertex[l_triangleDescription.Point2].xyz);

	float area0, area1, area2;

	area0 = GetTriangleArea(AllVertex[l_triangleDescription.Point1].xyz, AllVertex[l_triangleDescription.Point2].xyz, p_intersectPos);
	area1 = GetTriangleArea(AllVertex[l_triangleDescription.Point0].xyz, AllVertex[l_triangleDescription.Point2].xyz, p_intersectPos);
	area2 = GetTriangleArea(AllVertex[l_triangleDescription.Point0].xyz, AllVertex[l_triangleDescription.Point1].xyz, p_intersectPos);

	float b0, b1, b2;
	b0 = area0 / totalArea;
	b1 = area1 / totalArea;
	b2 = area2 / totalArea;
	
	float2 texcoord0 = AllTexCoord[l_triangleDescription.TexCoordIndex0];
	float2 texcoord1 = AllTexCoord[l_triangleDescription.TexCoordIndex1];
	float2 texcoord2 = AllTexCoord[l_triangleDescription.TexCoordIndex2];

	return b0*texcoord0 + b1*texcoord1 + b2*texcoord2 * l_triangleDescription.padding1;

	// http://www.ems-i.com/gmshelp/Interpolation/Interpolation_Schemes/Inverse_Distance_Weighted/Computation_of_Interpolation_Weights.htm
}

float4 GetTriangleTexture(in uint p_primitiveIndex, in float3 p_intersectPos)
{
	float2 uv = GetTriangleTextureCoordinates(p_primitiveIndex, p_intersectPos);

	return BoxTexture.SampleLevel(MeshTextureSampler, uv, 0);
}

float4 GetPrimitiveColor(in uint p_primitiveIndex, in uint p_primitiveType, in float3 p_intersectPos)
{
	if (p_primitiveType == PRIMITIVE_SPHERE)			// Sphere
		return float4(Sphere[p_primitiveIndex].color, 0.0f);
	else if (p_primitiveType == PRIMITIVE_TRIANGLE)		// Triangle
	{
		float4 add_color = float4(1.0f, 1.0f, 1.0f, 1.0f);
		return GetTriangleTexture(p_primitiveIndex, p_intersectPos) * add_color;
	}
	return BLACK4;
}

float GetReflectiveFactor(in uint p_primitiveIndex, in uint p_primitiveType)
{
	if (p_primitiveType == PRIMITIVE_SPHERE)			// Sphere
		return Sphere[p_primitiveIndex].material.reflectivefactor;
	else if (p_primitiveType == PRIMITIVE_TRIANGLE)		// Triangle
		return AllTriangleDesc[p_primitiveIndex].material.reflectivefactor;
	return 0.0f;
}

float GetReflective(in uint p_primitiveIndex, in uint p_primitiveType)
{
	if (p_primitiveType == PRIMITIVE_SPHERE)			// Sphere
		return Sphere[p_primitiveIndex].material.isReflective;
	else if (p_primitiveType == PRIMITIVE_TRIANGLE)		// Triangle
		return AllTriangleDesc[p_primitiveIndex].material.isReflective;
	return 0.0f;
}

float4 Shade(in Ray p_ray, in uint p_primitiveIndex, in uint p_primitiveType, in float4 p_collideNormal, in Material p_material)
{
	float4 l_illumination = float4(0.0f, 0.0f, 0.0f, 0.0f);

	for (uint i = 0; i < LIGHT_COUNT; i++) // for each light
	{	
		// Light and shadows
		bool l_isLitByLight = IsLitByLight(p_ray, p_primitiveIndex, p_primitiveType, i);
		if (l_isLitByLight == true) // Thus is lit
		{
			l_illumination += CalcLight(p_material, p_ray.origin, cameraPosition, p_collideNormal, PointLight[i]) * PointLight[i].color;
		}
	}

	l_illumination += float4(p_material.ambient, 1.0f);
	l_illumination *= GetPrimitiveColor(p_primitiveIndex, p_primitiveType, p_ray.origin.xyz);			// add color
	return l_illumination;
}

bool CloseToZero(in float p_float)
{
	if(p_float > -EPSILON && p_float < EPSILON )
		return true;
	return false;
}

#define max_number_of_bounces 2
float4 Trace(in Ray p_ray)
{
	Ray l_nextRay = p_ray;
	float4 colorIllumination = float4(0.0f, 0.0f, 0.0f, 0.0f);

	float4 l_collideNormal;
	float4 l_intersectPosition;
	Material l_material;
	uint l_primitiveIndex;
	uint l_primitiveType;

	l_nextRay = Jump(l_nextRay, l_collideNormal, l_material, l_primitiveIndex, l_primitiveType); // First jump. From screen to first object

	if (l_primitiveType != PRIMITIVE_NOTHING) // As long as it is SOMETHING 
	{
		colorIllumination += Shade(l_nextRay, l_primitiveIndex, l_primitiveType, l_collideNormal, l_material);	// Get illumination 
	}
	
	uint l_isReflective;
	float l_reflectiveFactor = 1.0f;
	for(uint i = 0; i < max_number_of_bounces-1; i++) // Iterate through all jump
	{
		l_isReflective = GetReflective(l_primitiveIndex, l_primitiveType);		// Get if the material is reflective (Used to check if next jump should be executed)					
		if(l_isReflective == 1) // Is is reflective
		{
			l_reflectiveFactor *= GetReflectiveFactor(l_primitiveIndex, l_primitiveType); // Get the reflectiveness on this material and multiplies with previuous reflectivefactor. Get this before next jump
			if(CloseToZero(l_reflectiveFactor) == true) // Breaks if the reflection is very close to zero
				break;
			l_nextRay = Jump(l_nextRay, l_collideNormal, l_material, l_primitiveIndex, l_primitiveType); // Jump to next object to get next color
			
			if(l_primitiveType != PRIMITIVE_NOTHING)	// See if the ray hit anything
			{
				colorIllumination += l_reflectiveFactor * Shade(l_nextRay, l_primitiveIndex, l_primitiveType, l_collideNormal, l_material); // Illuminate with new object with multiplied with last objects reflectiveFactor
			}
			else if(l_primitiveType == PRIMITIVE_NOTHING)	// If the ray hit NOTHING.
				break;										// Quit
		}
		else // Not reflective at all
			break;
	}
	return colorIllumination;
}

[numthreads(16, 16, 1)]
void RayTrace( uint3 threadID : SV_DispatchThreadID)
{
	float4 l_finalColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
	int x_coord = threadID.x + 400 * x_dispatch_count;
	int y_coord = threadID.y + 400 * y_dispatch_count; // 400 because making 1/16, 16 times times, of 1600x1600

	Ray l_ray = createRay(x_coord, y_coord);
	l_finalColor = Trace(l_ray);
		
	float a;
	// Normalizing after highest value	
	a = max(l_finalColor.x, l_finalColor.y);
	a = max(a, l_finalColor.z);
	a = max(a, 1.0f);
	l_finalColor /= a;
	
	int array_width = 1600;

	temp[x_coord + y_coord*array_width] = l_finalColor;
}

[numthreads(32, 32, 1)]
void RenderToBackBuffer(uint3 threadID : SV_DispatchThreadID)
{
	int2 coord;
	coord.x = threadID.x; 
	coord.y = threadID.y;
	
	int x = coord.x * 2;
	int y = ((coord.y * 2) * 1600) -1;

	float4 l_finalColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

	l_finalColor =	temp[x + y]				+ temp[x+1 + y]	+			// Topleft		// Topright
					temp[x + y + 1 * 1600]	+ temp[x+1 + y + 1*1600];	// Botleft		// Botright

	output[coord.xy] = l_finalColor/4.0f;
}