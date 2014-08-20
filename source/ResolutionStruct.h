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

		unsigned int RayTraceCallsX;
		unsigned int RayTraceCallsY;

		unsigned int AmountofThreadGroupWhenRaytracingX;
		unsigned int AmountofThreadGroupWhenRaytracingY;
		char* RayTraceFunctionName;

		unsigned int AmountofThreadGroupWhenRenderingX;
		unsigned int AmountofThreadGroupWhenRenderingY;
		char* RenderFunctionName;

		unsigned int RenderCallsX;
		unsigned int RenderCallsY;

		ResolutionData() {};
	};

	ResolutionData GetResolution(Resolution p_resolution) 
	{
		ResolutionData l_returnData;
		if(p_resolution == Resolution::A800x800) 
		{
			l_returnData.AmountofThreadGroupWhenRaytracingX = 25;
			l_returnData.AmountofThreadGroupWhenRaytracingY = 25;

			l_returnData.RayTraceCallsX = 4;
			l_returnData.RayTraceCallsY = 4;

			l_returnData.AmountofThreadGroupWhenRenderingX = 25;
			l_returnData.AmountofThreadGroupWhenRenderingY = 25;

			l_returnData.RenderCallsX = 1;
			l_returnData.RenderCallsY = 1;

			l_returnData.width = 800.0f;
			l_returnData.height = 800.0f;

			l_returnData.RayTraceFunctionName = "RayTrace16";
			l_returnData.RenderFunctionName = "RenderToBackBuffer";
		}
		else if(p_resolution == Resolution::A1024x1024) 
		{
			l_returnData.AmountofThreadGroupWhenRaytracingX = 32;
			l_returnData.AmountofThreadGroupWhenRaytracingY = 32;

			l_returnData.RayTraceCallsX = 4;
			l_returnData.RayTraceCallsY = 4;

			l_returnData.AmountofThreadGroupWhenRenderingX = 32;
			l_returnData.AmountofThreadGroupWhenRenderingY = 32;

			l_returnData.RenderCallsX = 4;
			l_returnData.RenderCallsY = 4;
			
			l_returnData.width = 1024.0f;
			l_returnData.height = 1024.0f;
			
			l_returnData.RayTraceFunctionName = "RayTrace32";
			l_returnData.RenderFunctionName = "RenderToBackBuffer";
		}
		return l_returnData;
	}
}