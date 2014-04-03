#ifndef LIGHT_H
#define LIGHT_H

#define LIGHT_COUNT 3

namespace CustomLightStruct
{
	using namespace DirectX;

	struct PointLightData
	{
		XMFLOAT4 position;
		XMFLOAT4 color;	
		XMFLOAT4 ambientLight;
		XMFLOAT4 diffuseLight;
		XMFLOAT3 specularLight;
		float	 lightRadius;
	};

	struct LightBuffer
	{
		PointLightData pointLight[LIGHT_COUNT];
	};
}


#endif