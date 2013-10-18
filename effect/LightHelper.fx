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