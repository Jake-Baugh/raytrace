#include "RayStruct.fx"

struct Material
{
	float ambient;
	float specular;
	float diffuse;
	float shininess;
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
    float4 l_ambient = M.ambient * p_ambient;//float4(0.5f, 0.5f, 0.5f, 0.5f);
    float4 l_diffuse = M.diffuse * saturate( dot(N,L) );
    float4 l_specular = M.specular * pow( saturate(dot(R,V)), M.shininess );
 
    return l_ambient + (l_diffuse + l_specular) * l_pointLight.color;
}

float4 CalcLight (Ray p_ray, PointLightData light, float4 hitPos, float3 hitNormal, Material M, float4 p_ambient)
//float4 calculateLight(Ray p_ray, pointLight light,		float4 hitPos, float3 hitNormal, material M)
{
	float3 lightDir = normalize(hitPos - light.position);
	float4 lightAttenuation = 1 / (length(lightDir) * length(lightDir));

	float3 N = normalize( hitNormal );
	float3 V = normalize( p_ray.origin - hitPos );
	float3 R = reflect( lightDir, N);

	return lightAttenuation * calcPhongLighting( M, N, lightDir, V, R, light, p_ambient);
}

