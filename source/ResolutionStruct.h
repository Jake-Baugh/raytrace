#pragma once


namespace Resolution
{
	enum Resolution
	{
		A800x800,
		A1024x1024,
		A800x800_WITH_SS
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
		
		boolean DoSupersamling; // If this is set to false, the variables below is not needed.
		unsigned int AmountofThreadGroupWhenRenderingX;
		unsigned int AmountofThreadGroupWhenRenderingY;
		char* RenderFunctionName;

		unsigned int RenderCallsX;
		unsigned int RenderCallsY;

		ResolutionData() 
		{
			DoSupersamling = false;
		};
	};

	ResolutionData GetResolution(Resolution p_resolution) 
	{
		ResolutionData l_returnData;
		if(p_resolution == Resolution::A800x800) 
		{
			l_returnData.AmountofThreadGroupWhenRaytracingX = 25;
			l_returnData.AmountofThreadGroupWhenRaytracingY = 25;

			l_returnData.RayTraceCallsX = 2;
			l_returnData.RayTraceCallsY = 2;

			l_returnData.width = 800.0f;
			l_returnData.height = 800.0f;
			l_returnData.DoSupersamling = false;

			l_returnData.RayTraceFunctionName = "RayTrace16AndRender";
			l_returnData.RenderFunctionName = "RenderToBackBuffer";
		}
		else if(p_resolution == Resolution::A1024x1024) 
		{
			l_returnData.DoSupersamling = false;
			l_returnData.AmountofThreadGroupWhenRaytracingX = 32;
			l_returnData.AmountofThreadGroupWhenRaytracingY = 32;

			l_returnData.RayTraceCallsX = 1;
			l_returnData.RayTraceCallsY = 1;
			
			l_returnData.width = 1024.0f;
			l_returnData.height = 1024.0f;
			
			l_returnData.RayTraceFunctionName = "RayTrace32AndRender";

			// Not used
			l_returnData.RenderFunctionName = "RenderToBackBuffer";
		}
		else if(p_resolution == Resolution::A800x800_WITH_SS) 
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
			l_returnData.DoSupersamling = true;

			l_returnData.RayTraceFunctionName = "RayTrace16AndPrepareMultisampling";
			l_returnData.RenderFunctionName = "RenderToBackBuffer";
		}
		
		return l_returnData;
	}
}