#pragma once


namespace Resolution
{
	enum Resolution
	{
		A800x800,
		A1024x1024
	};

	struct ResolutionData
	{
		float width;
		float height;

		unsigned int AmountOfXCalls;
		unsigned int AmountOfYCalls;

		unsigned int AmountofThreadGroupWhenRaytracingX;
		unsigned int AmountofThreadGroupWhenRaytracingY;
		
		unsigned int AmountofThreadGroupWhenRenderingX;
		unsigned int AmountofThreadGroupWhenRenderingY;

		ResolutionData() {};
	};

	ResolutionData GetResolution(Resolution p_resolution) 
	{
		ResolutionData l_returnData;
		if(p_resolution == Resolution::A800x800) 
		{
			l_returnData.AmountofThreadGroupWhenRaytracingX = 25;
			l_returnData.AmountofThreadGroupWhenRaytracingY = 25;

			l_returnData.AmountofThreadGroupWhenRenderingX = 25;
			l_returnData.AmountofThreadGroupWhenRenderingY = 25;

			l_returnData.AmountOfXCalls = 4;
			l_returnData.AmountOfYCalls = 4;

			l_returnData.height = 800.0f;
			l_returnData.height = 800.0f;
		}
		else if(p_resolution == Resolution::A1024x1024) 
		{
			l_returnData.AmountofThreadGroupWhenRaytracingX = 32;
			l_returnData.AmountofThreadGroupWhenRaytracingY = 32;

			l_returnData.AmountofThreadGroupWhenRenderingX = 32;
			l_returnData.AmountofThreadGroupWhenRenderingY = 32;

			l_returnData.AmountOfXCalls = 4;
			l_returnData.AmountOfYCalls = 4;

			l_returnData.height = 1024.0f;
			l_returnData.height = 1024.0f;
		}
		return l_returnData;
	}
}