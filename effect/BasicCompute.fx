//--------------------------------------------------------------------------------------
// BasicCompute.fx
// Direct3D 11 Shader Model 5.0 Demo
// Copyright (c) Stefan Petersson, 2012
//--------------------------------------------------------------------------------------

#pragma pack_matrix(row_major)

RWTexture2D<float4> output : register(u0);

cbuffer EveryFrameBuffer : register(c0) 
{
	float4	 cameraPosition;
	float4x4 inverseProjection;
	float4x4 inverseView;
	//float4 screenVariable;
	float screenWidth;
	float screenHeight;
	float padding1;
	float padding2;
}

/*
cbuffer PrimitivesBuffer: register(c1)
{
	float4	circleMidPos;
	float	circleRadius;
	float3	circleColor;
	float4  trianglePos1;
	float4  trianglePos2;
	float4  trianglePos3;
	float4  triangleColor;			//float4 for padding reasons = _ =
}
*/

struct SphereStruct
{
	float4	midPos;
	float3	color;
	float	radius;
};


struct TriangleStruct
{	
	float4  pos1;
	float4  pos2;
	float4  pos3;
	float4  color;			//float4 for padding reasons 
};

cbuffer PrimitiveBuffer: register(c1)
{
	SphereStruct	Sphere[2];
	TriangleStruct	Triangle[2];
}

struct Ray
{
	float4 origin;
	float4 direction;
	float4 color;
	float distance;
	bool hit;
};

Ray createRay(int x, int y)
{
	Ray l_ray;
	l_ray.origin = cameraPosition;
 
	double normalized_x = ((x / screenWidth.x) - 0.5) * 2;
	double normalized_y = (1 - (y / screenHeight) - 0.5) * 2;
 
	float4 imagePoint = mul(float4(normalized_x, normalized_y, 1, 1), inverseProjection);
	imagePoint /= imagePoint.w;
 
	imagePoint = mul(imagePoint, inverseView);
 
	l_ray.direction = imagePoint - l_ray.origin;
	l_ray.direction = normalize(l_ray.direction);
	l_ray.color = float4(0.0f, 0.0f, 0.0f, 1.0f);
	//l_ray.direction = float4(normalized_x, normalized_y, 0.0f, 1.0f);
	l_ray.hit = false;
	return l_ray;
}

float SphereIntersect(Ray p_ray, int index)		//Kod från lab1 3D1
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


float3 VecInverse(float3 p_vec)
{
	p_vec.x *= -1;
	p_vec.y *= -1;
	p_vec.z *= -1;
	return p_vec;
}

float Determinant(float3 p_vec1, float3 p_vec2, float3 p_vec3)
{
	float a =	p_vec1.x * p_vec2.y * p_vec3.z +
				p_vec1.y * p_vec2.z * p_vec3.x +
				p_vec1.z * p_vec2.x * p_vec3.y;

	float b =	p_vec1.z * p_vec2.y * p_vec3.x +
				p_vec1.y * p_vec2.x * p_vec3.z +
				p_vec1.x * p_vec2.z * p_vec3.y;

	return a-b;
}	

float veclen(float4 p_vec)
{
	float a = p_vec.x * p_vec.x + p_vec.y * p_vec.y + p_vec.z * p_vec.z;
	a = sqrt(a);
	return a;
}


float TriangleIntersect(Ray p_ray, int index)
{
	float3 edge1 = Triangle[index].pos2.xyz - Triangle[index].pos1.xyz;
	float3 edge2 = Triangle[index].pos3.xyz - Triangle[index].pos1.xyz;
	float3 l_distance = p_ray.origin.xyz - Triangle[index].pos1.xyz;

	float firstDet = Determinant(VecInverse(p_ray.direction), edge1, edge2);

	if(firstDet > -0.0000001 && firstDet < 0.0000001)
	{
		return 0.0f; // No hit
	}

	float secondDet	 = Determinant(l_distance, edge1, edge2);
	float thirdDet	 = Determinant(VecInverse(p_ray.direction), l_distance, edge2);
	float fourthDet	 = Determinant(VecInverse(p_ray.direction), edge1, l_distance);

	float t = secondDet * 1/firstDet;
	float u = thirdDet	* 1/firstDet;
	float v = fourthDet * 1/firstDet;

	if(u < 0 || v < 0 || u + v >= 1)
	{
		return 0.0f;
	}
	if(u < 0.00001f || v < 0.00001f)
		return 0.0f;

	float3 l_normal = cross(edge1, edge2);
	float plane_def = dot(l_normal, (float3)Triangle[index].pos1);

	float ta  =  - (dot(l_normal, p_ray.origin) + plane_def) / dot(l_normal, p_ray.direction); //http://www.scratchapixel.com/lessons/3d-basic-lessons/lesson-9-ray-triangle-intersection/ray-triangle-intersection-geometric-solution/
	
	float4 P = p_ray.origin + ta * p_ray.direction;
	float rvalue = veclen(P);
	return rvalue;

//return 1.0f;
}


Ray RayUpdate(Ray p_ray, int p_Bounces, int index)
{	
	// Variables used by all intersections
	float4 l_tempColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 l_collidePos;

	// Sphere specific variable
	float l_circlehit = 0.0f;
	float l_trianglehit = 0.0f;

	l_circlehit	  = SphereIntersect(p_ray, index);
	l_trianglehit = TriangleIntersect(p_ray, index);
	
	if(l_trianglehit == 0.0f && l_circlehit == 0.0f)
	{
			return p_ray; // It hit nothing, return direct
	}

	//////////////////////////////
	
	if(0.0f != l_circlehit && (l_trianglehit == 0.0f || l_trianglehit < 0.0f || l_circlehit < l_trianglehit))
	{
		l_tempColor = float4(Sphere[index].color.xyz, 1);
		p_ray.hit = true;
	}
	else if(0.0f != l_trianglehit && 0.0f < l_trianglehit && (l_circlehit == 0.0f || l_trianglehit < l_circlehit))
	{
		l_tempColor = Triangle[index].color;

		p_ray.hit = true;
	}
	else
	{
		l_tempColor = float4(0.0f, 0.0f, 1.0f, 1.0f);
	}

	
			
	////////////////////////////// ---------- End of 1

/*	
	if(l_trianglehit == 0.0f)
	{
		if(l_circlehit != 0.0f)
		{
			l_tempColor = float4(Circle[index].circleColor, 1);
		}
	}
	else if(l_circlehit == 0.0f)
	{
		if(l_trianglehit != 0.0f)
		{
			l_tempColor = Triangle[index].triangleColor;
		}
	}	
	else if(l_trianglehit != 0.0f && l_circlehit != 0.0f)
	{
		if(l_trianglehit < l_circlehit)
			//l_tempColor = triangleColor;
		//if(l_circlehit < l_trianglehit)
			l_tempColor = float4(0.0f, 0.0f, 1.0f, 1.0f);
	}
*/
	////////////////////////////// ---------- End of 2

	//float t_trianglehit;
	//float4 t_triangleHitData;
	//float4 t_triangleNormal;

	//This is so totaly wrong, should be a for loop that changes the Ray pos every time and each intersection test should just use that pos... i think, maybe..hm yes exactly...
	//float4 circleMid = mul(Circle_MidPos, WVPMatrix);
	//http://www.scratchapixel.com/lessons/3d-basic-lessons/lesson-9-ray-triangle-intersection/ray-triangle-intersection-geometric-solution/
	//http://inst.eecs.berkeley.edu/~cs184/fa09/raytrace_journal.php
	//http://paulbourke.net/geometry/reflected/

	//t_triangleHitData = TriangleIntersect(p_ray);
	//t_triangleNormal = float4(t_triangleHitData.xyz,0);
	//t_trianglehit = t_triangleHitData.w;,

	//if(t_trianglehit != 0.0f)
	//{
		/*l_tempColor = (Triangle_Color);
		//Bounce ray booooounce
		l_collidePos = p_ray.Position + t_trianglehit * p_ray.Direction;
		p_ray.Position = l_collidePos;
		float4 l_collideNormal = normalize( Circle_MidPos-  p_ray.Position);
			p_ray.Direction = reflect(p_ray.Direction, l_collideNormal); */
	//}

	//	l_tempColor = (float4(circleColor, 1));
	/*float b = TriangleIntersect(p_ray);
	if(b != 0.0f)
	{
		l_tempColor = triangleColor;
	}*/

	p_ray.color += l_tempColor;

	return p_ray;
}

#define max_number_of_bounces 2
[numthreads(32, 32, 1)]
void main( uint3 threadID : SV_DispatchThreadID)
{
	float4 l_finalColor = float4(0,0,0,1);
	Ray l_ray = createRay(threadID.xy, threadID.y);
		
	for(int i = 1; i < max_number_of_bounces; i++)	
	{
	//	l_ray = RayUpdate(l_ray, i, i-1);
	}
	l_ray = RayUpdate(l_ray, 1, 0);
	l_ray = RayUpdate(l_ray, 1, 1);

	if(l_ray.hit == false)
		l_ray.color = float4(0.0f, 0.0f, 0.0f, 1.0f);


	output[threadID.xy] = l_ray.color;
	//output[threadID.xy] = l_ray.direction; //Debug thingy sak för att se om rays faktiskt blir nåt	
}