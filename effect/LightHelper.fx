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

float4 calcPhongLighting(Material M, float4 L, float4 N, float4 R, float4 V)
{
	float4 l_diffuse	= float4(M.diffuse, 1.0f) * saturate(dot(N, L)) * diffuseLight;
	float4 l_specular	= float4(M.specular, 1.0f) * pow(saturate(dot(R, V)), M.shininess) * specularLight;
 
    return l_diffuse + l_specular;
}

float4 CalcLight(Material M, float4 HitPosition, float4 LightPosition, float4 CameraPosition, float4 SurfaceNormal)
{	
	float4 L = normalize(LightPosition - HitPosition); // vectorTowardsLightFromHit
	float4 N = normalize(SurfaceNormal);

	//	float3 R = normalize(reflect(L, N));
	float4 R = normalize(2 * saturate(dot(L, N)) * N - L);
	float4 V = normalize(CameraPosition - HitPosition); // vectorTowardsCameraFromHit

	float4 returnVector = calcPhongLighting(M, L, N, R, V);

	return returnVector;
}

