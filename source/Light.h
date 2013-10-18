#ifndef LIGHT_H
#define LIGHT_H

namespace CustomLightStruct
{
	struct LightData
	{
		D3DXVECTOR4 ambient;
		D3DXVECTOR4 diffuse;
		D3DXVECTOR4 specular;
		D3DXVECTOR4 attenuation; // attenuation parameters (a0, a1, a2)
		D3DXVECTOR3 position;
		float range;
	};

	struct SurfaceInfo
	{
		D3DXVECTOR4 posistion;
		D3DXVECTOR4 normal;
		D3DXVECTOR4 diffuse;
		D3DXVECTOR4 specular;
	};

	struct AllLight
	{
		LightData Light[1];
	};
}


#endif