#ifndef LIGHT_H
#define LIGHT_H

#define LIGHT_COUNT 9

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
		float lightCount;
		D3DXVECTOR3 ambientLight;
		PointLightData pointLight[LIGHT_COUNT];
	};
}


#endif