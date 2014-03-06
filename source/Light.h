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
	};

	struct LightBuffer
	{
		float lightCount;
		XMFLOAT3 ambientLight;
		PointLightData pointLight[LIGHT_COUNT];
	};
}


#endif