#ifndef LIGHT_H
#define LIGHT_H

namespace CustomLightStruct
{
	/*
	struct LightData
	{
		D3DXVECTOR4 ambient;
		D3DXVECTOR4 diffuse;
		D3DXVECTOR4 specular;
		D3DXVECTOR4 attenuation; // attenuation parameters (a0, a1, a2)
		D3DXVECTOR3 position;
		float range;
	};*/

	struct Material
	{
		float ambient;
		float specular;
		float diffuse;
		float shininess;
	};

	/*
	struct DirectionalLight
	{
		D3DXVECTOR4 color;
		D3DXVECTOR4 direction;
	};*/

	struct PointLightData
	{
		D3DXVECTOR4 position;
		D3DXVECTOR4 color;
	};

	struct LightBuffer
	{
		PointLightData PointLight[1];
		D3DXVECTOR4 ambientLight;
	};
}


#endif