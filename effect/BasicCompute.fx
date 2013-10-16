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


struct CircleStruct
{
	float4	circleMidPos;
	float3	circleColor;
	float	circleRadius;
};


struct TriangleStruct
{
	
	float4  trianglePos1;
	float4  trianglePos2;
	float4  trianglePos3;
	float4  triangleColor;			//float4 for padding reasons 
	
};

cbuffer PrimitiveBuffer: register(c1)
{
	CircleStruct	Circle[2];
	TriangleStruct	Triangle[2];
}

struct Ray
{
	float4 origin;
	float4 direction;
	float4 color;
	float distance;
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
	return l_ray;
}


float SphereIntersect(Ray p_ray, float4 p_circleMid, int index)		//Kod från lab1 3D1
{	
	float4 l_distance = p_ray.origin - p_circleMid;
	float a, b, t, t1, t2;
	b = dot(p_ray.direction, l_distance);
	a = dot(l_distance, l_distance ) - (Circle[index].circleRadius * Circle[index].circleRadius);
//	a = dot(l_distance, l_distance ) - (circleRadius * circleRadius);

	if(b * b - a >= 0)
	{
		t = sqrt(b * b -a);
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


//float4  Triangle_Pos1;
//	float4  Triangle_Pos2;
//	float4  Triangle_Pos3;
//	float4  Triangle_Color;	

float3 VecInverse(float3 p_vec)
{
	p_vec.x *= -1;
	p_vec.y *= -1;
	p_vec.z *= -1;
	return p_vec;
}

static float Determinant(float3 p_vec1, float3 p_vec2, float3 p_vec3)
{
	float a =	p_vec1.x * p_vec2.y * p_vec3.z +
				p_vec1.y * p_vec2.z * p_vec3.x +
				p_vec1.z * p_vec2.x * p_vec3.y;

	float b =	p_vec1.z * p_vec2.y * p_vec3.x +
				p_vec1.y * p_vec2.x * p_vec3.z +
				p_vec1.x * p_vec2.z * p_vec3.y;

	return a-b;
}	

float TriangleIntersect(Ray p_ray, int index)			//Kod från http://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
{
	
	float3 e1 = Triangle[index].trianglePos2.xyz - Triangle[index].trianglePos1.xyz;
	float3 e2 = Triangle[index].trianglePos3.xyz - Triangle[index].trianglePos1.xyz;
	float3 l_distance = p_ray.origin.xyz - Triangle[index].trianglePos1.xyz;
	

	/*
	float3 e1 = trianglePos2.xyz - trianglePos1.xyz;
	float3 e2 = trianglePos3.xyz - trianglePos1.xyz;
	float3 l_distance = p_ray.origin.xyz - trianglePos1.xyz;
	*/

	float3 firstDet = Determinant(VecInverse(p_ray.direction), e1, e2);
	return 1.0f; // HERE
}

Ray RayUpdate(Ray p_ray, int p_Bounces, int index)
{	
	// Variables used by all intersections
	float4 l_tempColor = float4(0,0,0,1);
	float4 l_collidePos;

	// Sphere specific variable
	float t_circlehit;
	
//	t_circlehit = SphereIntersect(p_ray, circleMidPos);
	t_circlehit = SphereIntersect(p_ray, Circle[index].circleMidPos, index);
	if(/*t_trianglehit != 0.0f ||*/ t_circlehit != 0.0f)
	{
		//if((t_circlehit < t_trianglehit && t_circlehit != 0.0f)  || t_trianglehit == 0.0f )
			l_tempColor = float4(Circle[index].circleColor, 1.0f);
		//	l_tempColor = float4(circleColor, 1.0f);
			//bounce the ray   ## This seems to glitch at about half of the rays, abit odd mmyesss
			l_collidePos = p_ray.origin + t_circlehit * p_ray.direction;
			p_ray.origin = l_collidePos;
			float4 l_collideNormal  = normalize( Circle[index].circleMidPos - p_ray.origin);
		//	float4 l_collideNormal  = normalize( circleMidPos - p_ray.origin);
			p_ray.direction			= reflect(p_ray.direction, l_collideNormal);
	}


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


	output[threadID.xy] = l_ray.color;
	//output[threadID.xy] = l_ray.direction; //Debug thingy sak för att se om rays faktiskt blir nåt	
}