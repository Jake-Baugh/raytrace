#include "RayStruct.fx"

struct Material
{
	float ambient;
	float specular;
	float diffuse;
	float shininess;
	float reflective;
	float refractive;
	float isReflective;
	float isRefractive;
};

struct PointLightData
{
	float4 position;
	float4 color;
};


//--------------------------------------------------------------------------------------
// Phong Lighting Reflection Model
//--------------------------------------------------------------------------------------
float4 calcPhongLighting( Material M, float3 N, float3 L, float3 V, float3 R, PointLightData l_pointLight, float4 p_ambient)
{
    float4 l_ambient = M.ambient * p_ambient;
    float4 l_diffuse = M.diffuse * saturate( dot(N,L) );
    float4 l_specular = M.specular * pow( saturate(dot(R,V)), M.shininess );
 
    return l_ambient + (l_diffuse + l_specular) * l_pointLight.color;
}

float4 CalcLight (Ray p_ray, PointLightData light, float4 hitPos, float3 hitNormal, Material M, float4 p_ambient)
{
	float3 lightDir = normalize(hitPos - light.position);
	float4 lightAttenuation = 1 / (length(lightDir) * length(lightDir));

	float3 NormalizedHitNormal = normalize( hitNormal );
	float3 V = normalize( p_ray.origin - hitPos );
	float3 R = reflect( lightDir, NormalizedHitNormal );

	return lightAttenuation * calcPhongLighting( M, NormalizedHitNormal, lightDir, V, R, light, p_ambient);
}

