#include "RayStruct.fx"
#include "Utilities.fx"


//--------------------------------------------------------------------------------------
// Phong Lighting Reflection Model
//--------------------------------------------------------------------------------------
/*
L, which is the direction vector from the point on the surface toward each light source (m specifies the light source),
N, which is the normal at this point on the surface,
R, which is the direction that a perfectly reflected ray of light would take from this point on the surface,
V, which is the direction pointing towards the viewer (such as a virtual camera)

// Source
http://en.wikipedia.org/wiki/Phong_reflection_model#Description
*/

float4 calcPhongLighting(Material M, float4 L, float4 N, float4 R, float4 V, PointLightData l_lightData)
{
	float4 l_ambient = float4(M.ambient, 1.0f) * ambientLight;
		float4 l_diffuse = float4(M.diffuse, 1.0f) * saturate(dot(L, N));
		//	float4 l_specular = float4(M.specular, 1.0f) * pow(max(dot(R, V), 0.0f), M.shininess);

		return l_ambient + l_diffuse;// +l_specular; //* l_lightData.color;
}

float4 CalcLight(Material M, float4 HitPosition, float4 CameraPosition, float4 SurfaceNormal, PointLightData l_lightData)
{	
	float4 L = normalize(l_lightData.position - HitPosition); // vectorTowardsLightFromHit
	float lightAttenuation = 1 / (length(L) * length(L));	// 1 / 
	
	float4 N = normalize(SurfaceNormal);

	//	float4 R = normalize(2 * saturate(dot(L, N)) * N - L);
		//float4 R = normalize(reflect(-L, N));
	float4 R = normalize(L - 2 * saturate(dot(L, N)) * N);
	
	float4 V = normalize(CameraPosition - HitPosition); // vector Towards Camera From Intersection Point

	return calcPhongLighting(M, L, N, R, V, l_lightData);
}

