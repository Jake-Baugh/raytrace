#include "RayStruct.fx"
#include "Utilities.fx"

/*	COMMENT 1
L, which is the direction vector from the point on the surface toward each light source (m specifies the light source),
N, which is the normal at this point on the surface,
R, which is the direction that a perfectly reflected ray of light would take from this point on the surface,
V, which is the direction pointing towards the viewer (such as a virtual camera)

// Source
http://en.wikipedia.org/wiki/Phong_reflection_model#Description
*/

/* COMMENT 2
Your specular contribution is actually going to be the same for front and back faces because GLSL reflect is insensitive to the sign of the normal. From this reference:

reflect(I, N) = I - 2.0 * dot(N, I) * N
so flipping the sign of N introduces two minus signs which cancel. In words, what reflect does is to reverse the sign of the component of I which is along the same axis as N. This doesn't depend on which way N is facing.

If you want to remove the specular component from your back faces I think you'll need to explcitly check your dot(vE,vN)
http://stackoverflow.com/questions/20008089/specular-lighting-appears-on-both-eye-facing-and-rear-sides-of-object

*/

/*
fAtt = 1/(a*d^2 + b*d + c)

d = distance between the light and the surface being shaded
a = quadratic attenuation factor
b = linear attenuation factor
c = constant attenuation factor

http://imdoingitwrong.wordpress.com/2011/01/31/light-attenuation/
*/


//--------------------------------------------------------------------------------------
// Phong Lighting Reflection Model
//--------------------------------------------------------------------------------------
float4 calcPhongLighting(Material M, float4 L, float4 N, float4 R, float4 V)
{
	float4 l_specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

	float4 l_diffuse = saturate(dot(L, N));

	if (l_diffuse.x > 0.0f)	// Read COMMENT 2
		l_specular = float4(M.specular, 1.0f) * pow(saturate(dot(R, V)), M.shininess);

	l_diffuse *= float4(M.diffuse, 1.0f);

	return (l_diffuse + l_specular);
}

float4 CalcLight(Material M, float4 HitPosition, float4 CameraPosition, float4 SurfaceNormal, PointLightData l_lightData)
{	
	float4 L = l_lightData.position - HitPosition; // vectorTowardsLightFromHit
	float d = length(L);

	float r = 5000.0f; //l_lightData.lightRadius;	
	float a = 1.0f / (r*r);
	float b = 2.0f/r;
	float c = 1.0f;

	float lightAttenuation = 1 / (a*d*d + b*d + c); // Read COMMENT 3
	L = normalize(L);

	float4 N = normalize(SurfaceNormal);
	float4 R = normalize(2 * saturate(dot(L, N)) * N - L);
	float4 V = normalize(CameraPosition - HitPosition); // vector Towards Camera From Intersection Point

	return lightAttenuation * calcPhongLighting(M, L, N, R, V);
}

