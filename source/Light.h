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
		XMFLOAT3 ambientLight;
		float lightCount;
		XMFLOAT3 diffuseLight;
		float PADDING1;
		XMFLOAT3 specularLight;
		float PADDING2;
		PointLightData pointLight[LIGHT_COUNT];
	};
}


#endif