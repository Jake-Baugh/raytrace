#include "RayStruct.fx"

float4 ambientLight;

struct LightData
{
	float4 ambient;
	float4 diffuse;
	float4 specular;
	float4 attenuation;
	float3 position;	
	float range;
};

struct SurfaceInfo
{
	float4 position;
	float4 normal;
	float4 diffuse;
	float4 specular;
};

struct DirectionalLight
{
	float4 color;
	float4 direction;
};

struct Material
{
	float ambient;
	float specular;
	float diffuse;
	float shininess;
};

// Link to look at! http://takinginitiative.wordpress.com/2010/08/30/directx-10-tutorial-8-lighting-theory-and-hlsl/
//-----------------------------------------------------------------------------------------
// Method Declaration
//-----------------------------------------------------------------------------------------
float4 PointLight(SurfaceInfo v, LightData L, float4 eyePos)
{
	float4 litColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// The vector from the surface to the light.
	float4 lightVec = float4(L.position, 0.0f) - v.position;

	// The distance from surface to light.
	float d = length(lightVec);

	if( d > L.range )
		return float4(0.0f, 0.0f, 0.0f, 0.0f);

	// Normalize the light vector.
	lightVec /= d;

	// Add the ambient light term.
	litColor += v.diffuse * L.ambient;

	// Add diffuse and specular term, provided the surface is in
	// the line of sight of the light.
	float diffuseFactor = dot(lightVec, v.normal);
	[branch]
	if( diffuseFactor > 0.0f )
	{
		float specPower = max(v.specular.a, 1.0f);
		float4 toEye = normalize(eyePos - v.position);
		float4 R = reflect(-lightVec, v.normal);
		float specFactor = pow(max(dot(R, toEye), 0.0f), specPower);

		// diffuse and specular terms
		litColor += diffuseFactor * v.diffuse * L.diffuse;
		litColor += specFactor * v.specular * L.specular;
	}

	// attenuate
	return litColor / dot(L.attenuation.xyz, float3(1.0f, d, d*d));
}


//--------------------------------------------------------------------------------------
// Phong Lighting Reflection Model
//--------------------------------------------------------------------------------------
float4 calcPhongLighting( Material M, float4 LColor, float3 N, float3 L, float3 V, float3 R )
{
    float4 l_ambient = M.ambient * float4(0.3f, 0.0f, 0.0f, 0.0f);
    float4 l_diffuse = M.diffuse * saturate( dot(N,L) );
    float4 l_specular = M.specular * pow( saturate(dot(R,V)), M.shininess );
 
    return l_ambient + (l_diffuse + l_specular) * LColor;
}

float4 CalcLight(Ray p_ray, DirectionalLight p_directionalLight, float3 p_normal, float3 p_cameraPosition)
{
	float4 l_output;

   	float3 l_normal = p_normal;
    float3 V = normalize( p_cameraPosition - p_ray.origin.xyz );
    float3 R = reflect( p_directionalLight.direction, l_normal);

	Material l_material;
	l_material.ambient		= 1.0f;
	l_material.specular		= 1.0f;
	l_material.diffuse		= 1.0f;
	l_material.shininess	= 1.0f;

	float3 l_lightDirection = normalize( p_ray.origin.xyz - p_directionalLight.direction);

    l_output = calcPhongLighting( l_material, p_directionalLight.color, l_normal, -p_directionalLight.direction, V, R);
  //l_output = calcPhongLighting( l_material, p_directionalLight.color, l_normal, l_lightDirection, V, R);

 
    return l_output;
}

