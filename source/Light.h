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
		XMFLOAT4 ambientLight;
//		float lightCount;
//		XMFLOAT4 diffuseLight;
//		float PADDING1;
//		XMFLOAT4 specularLight;
//		float PADDING2;
		PointLightData pointLight[LIGHT_COUNT];
	};
}


#endif