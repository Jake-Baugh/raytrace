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

float3 calcPhongLighting(Material M, float3 L, float3 N, float3 R, float3 V)
{
	float3 l_diffuse	= M.diffuse * saturate(dot(L, N)) * diffuseLight;
	float3 l_specular	= M.specular * pow(saturate(dot(R, V)), M.shininess) * specularLight;
 
    return l_diffuse + l_specular;
}

float4 CalcLight(Material M, float3 HitPosition, float3 LightPosition, float3 CameraPosition, float3 SurfaceNormal)
{
	float3 L = normalize(LightPosition - HitPosition); // vectorTowardsLightFromHit
	float3 N = normalize(SurfaceNormal);

	float3 R = normalize(2 * saturate(dot(N, L)) * N - L);
	float3 V = normalize(CameraPosition - HitPosition); // vectorTowardsCameraFromHit

	float3 returnVector = calcPhongLighting(M, L, N, R, V);

	return float4(returnVector, 1.0f);
}

