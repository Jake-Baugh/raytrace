#ifndef LIGHT_H
#define LIGHT_H

#define LIGHT_COUNT 3

namespace CustomLightStruct
{

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