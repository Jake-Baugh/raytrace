#ifndef LIGHT_H
#define LIGHT_H

namespace CustomLightStruct
{
	struct Material
	{
		float ambient;
		float specular;
		float diffuse;
		float shininess;
	};

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