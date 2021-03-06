//#include "stdafx.h"
#include <vector>
#include "ComputeHelp.h"
#include "D3D11Timer.h"
#include "Primitives.h"
#include "Light.h"
#include "Camera.h"
#include "ObjectLoader.h"
#include "DDSTextureLoader/DDSTextureLoader.h"
#include "ResolutionStruct.h"

#define MOUSE_SENSE 0.00087266f
#define MOVE_SPEED  450.0f

struct OnePerDispatch
{
	int x_dispatch_count;
	int y_dispatch_count;
	float client_width;
	float client_height;
};

/*
	This file is a mess.
	Functionss definitions in wrong order, no real naming conventions, huge functions and functionality in wrong places.
	Most things could've been moved out from this file. Buuuuuut, hey. You tell yourself it's easier if you put everything in the same file while learning new things.
*/

//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------
HINSTANCE					g_hInst					= NULL;  
HWND						g_hWnd					= NULL;

IDXGISwapChain*				g_SwapChain				= NULL;
ID3D11Device*				g_Device				= NULL;
ID3D11DeviceContext*		g_DeviceContext			= NULL;

ID3D11UnorderedAccessView*	g_BackBufferUAV			= NULL;		
ID3D11Buffer*				g_EveryFrameBuffer		= NULL; 	
ID3D11Buffer*				g_PrimitivesBuffer		= NULL;
ID3D11Buffer*				g_LightBuffer			= NULL;
ID3D11Buffer*				g_vertexBuffer			= nullptr;
ID3D11Buffer*				g_TexCoordBuffer		= nullptr;
ID3D11Buffer*				g_objectBuffer			= nullptr;
ID3D11Buffer*				g_normalBuffer			= nullptr;
ID3D11Buffer*				g_dispatchBuffer		= nullptr;
ID3D11Buffer*				g_tempBuffer			= nullptr;	
ID3D11Buffer*				g_smallBoxTexBuffer		= nullptr;
ID3D11UnorderedAccessView*	g_tempUAV				= nullptr;
ID3D11Buffer*				g_gpuPickingRayBuffer	= nullptr;
ID3D11SamplerState*			g_samplerState			= nullptr;

// Triangle mesh variables
std::vector<XMFLOAT4> g_allTrianglesVertex;// = nullptr;
std::vector<XMFLOAT2> g_allTrianglesTexCoord;// = nullptr;
std::vector<CustomPrimitiveStruct::TriangleDescription> g_allTrianglesIndex;
std::vector<XMFLOAT3> g_allTriangleNormal;

ID3D11ShaderResourceView* g_Vertex_SRV			= nullptr; 
ID3D11ShaderResourceView* g_TexCoord_SRV		= nullptr;
ID3D11ShaderResourceView* g_TriangleDesc_SRV	= nullptr;
ID3D11ShaderResourceView* g_Normal_SRV			= nullptr;
ID3D11ShaderResourceView* g_smallBoxTexSRV		= nullptr;

ID3D11ShaderResourceView* g_GpuRay_SRV			= nullptr;

ComputeShader*				RayTracingRender	= nullptr;
ComputeShader*				SuperSampleRender	= nullptr;

D3D11Timer*					g_Timer				= NULL;
int							g_Width, g_Height;
int							g_cameraIndex		= 0;
bool						g_mouse_clicked		= false;
POINT						m_oldMousePos;
OnePerDispatch				g_OnePerDispatch;

Resolution::ResolutionData	g_resolutionData;

//--------------------------------------------------------------------------------------
// Forward declarations
//--------------------------------------------------------------------------------------
HRESULT             InitWindow( HINSTANCE hInstance, int nCmdShow );
HRESULT				Init();
HRESULT				InitializeDXDeviceAndSwapChain();
HRESULT				CreatePrimitiveBuffer();
void				FillPrimitiveBuffer(float l_deltaTime);
HRESULT				CreateLightBuffer();
void				FillLightBuffer();
HRESULT				LoadMesh(char* p_path);
HRESULT				LoadObjectData();
HRESULT				CreateObjectBuffer();
HRESULT				CreateCameraBuffer();
void				FillCameraBuffer();
HRESULT				CreateDispatchBuffer();
void				UpdateDispatchBuffer(int l_x, int l_y);
HRESULT				CreateTempBufferAndUAV();
HRESULT				SetSmallBoxTexture();
HRESULT				Render(float deltaTime);
HRESULT				Update(float deltaTime);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
char*				FeatureLevelToString(D3D_FEATURE_LEVEL featureLevel);
HRESULT				SetSampler();

//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------
int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
	// FIRST THING TO DO, CHOOSE RESOLUTION
	g_resolutionData = Resolution::GetResolution(Resolution::A800x800);
	POINT p;
				p.x = g_resolutionData.width / 2;
			p.y = g_resolutionData.height / 2;
			ClientToScreen(g_hWnd, &p);
			SetCursorPos(p.x,p.y);

	if( FAILED( InitWindow( hInstance, nCmdShow ) ) )
		return 0;

	if( FAILED( Init() ) )
		return 0;

	__int64 cntsPerSec = 0;
	QueryPerformanceFrequency((LARGE_INTEGER*)&cntsPerSec);
	float secsPerCnt = 1.0f / (float)cntsPerSec;

	__int64 prevTimeStamp = 0;
	QueryPerformanceCounter((LARGE_INTEGER*)&prevTimeStamp);

	// Main message loop
	MSG msg = {0};
	while(WM_QUIT != msg.message)
	{
		if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		else
		{
			__int64 currTimeStamp = 0;
			QueryPerformanceCounter((LARGE_INTEGER*)&currTimeStamp);
			float dt = (currTimeStamp - prevTimeStamp) * secsPerCnt;

			//render
			Update(dt);
			Render(dt);

			prevTimeStamp = currTimeStamp;
		}
	}

	return (int) msg.wParam;
}
//--------------------------------------------------------------------------------------
// Register class and create window
//--------------------------------------------------------------------------------------
HRESULT InitWindow( HINSTANCE hInstance, int nCmdShow )
{
	// Register class
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX); 
	wcex.style          = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc    = WndProc;
	wcex.cbClsExtra     = 0;
	wcex.cbWndExtra     = 0;
	wcex.hInstance      = hInstance;
	wcex.hIcon          = 0;
	wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName   = NULL;
	wcex.lpszClassName  = "BTH_D3D_Template";
	wcex.hIconSm        = 0;
	if( !RegisterClassEx(&wcex) )
		return E_FAIL;

	// Create window
	g_hInst = hInstance; 
	RECT rc = { 0, 0, g_resolutionData.width, g_resolutionData.height};
	AdjustWindowRect( &rc, WS_OVERLAPPEDWINDOW, FALSE );
	
	if(!(g_hWnd = CreateWindow("BTH_D3D_Template", "BTH - Direct3D 11.0 Template",
							WS_OVERLAPPEDWINDOW,
							CW_USEDEFAULT, CW_USEDEFAULT,
							rc.right - rc.left,
							rc.bottom - rc.top,
							NULL, NULL, hInstance, NULL)))
	{
		return E_FAIL;
	}

	ShowWindow( g_hWnd, nCmdShow );

	return S_OK;
}

//--------------------------------------------------------------------------------------
// Create Direct3D device and swap chain
//--------------------------------------------------------------------------------------
HRESULT Init()
{	
	HRESULT hr;
	hr = InitializeDXDeviceAndSwapChain();
	if(FAILED(hr))	return hr;
	
	
	// My things
	g_allTrianglesVertex	= std::vector<XMFLOAT4>();
	g_allTrianglesTexCoord	= std::vector<XMFLOAT2>();
	g_allTrianglesIndex		= std::vector<CustomPrimitiveStruct::TriangleDescription>();
	
 	hr = CreateCameraBuffer();
	if(FAILED(hr))	
		return hr;

	hr = CreatePrimitiveBuffer();
	if(FAILED(hr))	
		return hr;

	hr = CreateLightBuffer();
	if(FAILED(hr))	
		return hr;
	 FillLightBuffer();

	hr = LoadObjectData();
	if(FAILED(hr))	
		return hr;

	hr = CreateObjectBuffer();
	if(FAILED(hr))	
		return hr;

	hr = CreateDispatchBuffer();
	if (FAILED(hr))
		return hr;

	hr = CreateTempBufferAndUAV();
	if (FAILED(hr))
		return hr;

	hr = SetSmallBoxTexture();
	if (FAILED(hr))
		return hr;

	FillPrimitiveBuffer(0.0f);

	hr = SetSampler();
	if(FAILED(hr))
		return hr;

	return S_OK;
}

HRESULT InitializeDXDeviceAndSwapChain()
{
	HRESULT hr = S_OK;;
	RECT rc;
	GetClientRect( g_hWnd, &rc );
	g_Width = rc.right - rc.left;;
	g_Height = rc.bottom - rc.top;

	UINT createDeviceFlags = 0;
	#ifdef _DEBUG
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
	#endif

	D3D_DRIVER_TYPE driverType;

	D3D_DRIVER_TYPE driverTypes[] = 
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	UINT numDriverTypes = sizeof(driverTypes) / sizeof(driverTypes[0]);

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory( &sd, sizeof(sd) );
	sd.BufferCount = 1;
	sd.BufferDesc.Width = g_Width;
	sd.BufferDesc.Height = g_Height;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_UNORDERED_ACCESS;
	sd.OutputWindow = g_hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	D3D_FEATURE_LEVEL featureLevelsToTry[] = {
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0
	};
	D3D_FEATURE_LEVEL initiatedFeatureLevel;

	for( UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++ )
	{
		driverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDeviceAndSwapChain(
			NULL,
			driverType,
			NULL,
			createDeviceFlags,
			featureLevelsToTry,
			ARRAYSIZE(featureLevelsToTry),
			D3D11_SDK_VERSION,
			&sd,
			&g_SwapChain,
			&g_Device,
			&initiatedFeatureLevel,
			&g_DeviceContext);

		if( SUCCEEDED( hr ) )
		{
			char title[256];
			sprintf_s(
				title,
				sizeof(title),
				"BTH - Direct3D 11.0 Template | Direct3D 11.0 device initiated with Direct3D %s feature level",
				FeatureLevelToString(initiatedFeatureLevel)
			);
			SetWindowText(g_hWnd, title);

			break;
		}
	}
	if( FAILED(hr) )
		return hr;

	// Create a render target view
	ID3D11Texture2D* pBackBuffer;
	hr = g_SwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), (LPVOID*)&pBackBuffer );
	if( FAILED(hr) )
		return hr;

	// create shader unordered access view on back buffer for compute shader to write into texture
	hr = g_Device->CreateUnorderedAccessView( pBackBuffer, NULL, &g_BackBufferUAV );

	//create helper sys and compute shader instance
	RayTracingRender = new ComputeShader();
	hr = RayTracingRender->Init(L"effect\\BasicCompute.fx", NULL, g_resolutionData.RayTraceFunctionName, NULL, g_Device, g_DeviceContext);

	SuperSampleRender = new ComputeShader();
	hr = SuperSampleRender->Init(L"effect\\BasicCompute.fx", NULL, g_resolutionData.RenderFunctionName, NULL, g_Device, g_DeviceContext);

	if (FAILED(hr))
		return hr;

	g_Timer = new D3D11Timer(g_Device, g_DeviceContext);
	return S_OK;
}

HRESULT CreateCameraBuffer()
{
	HRESULT hr = S_OK;

	int ByteWidth;

	D3D11_BUFFER_DESC CameraData;
	CameraData.BindFlags			=	D3D11_BIND_CONSTANT_BUFFER ;
	CameraData.Usage				=	D3D11_USAGE_DYNAMIC; 
	CameraData.CPUAccessFlags		=	D3D11_CPU_ACCESS_WRITE;
	CameraData.MiscFlags			=	0;
	ByteWidth						=	sizeof(CustomPrimitiveStruct::EachFrameDataStructure);
	CameraData.ByteWidth			=	ByteWidth;
	hr = g_Device->CreateBuffer( &CameraData, NULL, &g_EveryFrameBuffer);

	return hr;
}

void FillCameraBuffer()
{
	using namespace DirectX;
	
	XMFLOAT4X4 l_projection, l_view, l_inverseProjection, l_inverseView;
	l_projection	= Camera::GetCamera(g_cameraIndex)->GetProj();
	l_view			= Camera::GetCamera(g_cameraIndex)->GetView();

	XMStoreFloat4x4(&l_inverseProjection,	XMMatrixInverse(nullptr, XMLoadFloat4x4(&l_projection)));
	XMStoreFloat4x4(&l_inverseView,			XMMatrixInverse(nullptr, XMLoadFloat4x4(&l_view)));

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	g_DeviceContext->Map(g_EveryFrameBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	CustomPrimitiveStruct::EachFrameDataStructure l_eachFrameData;
	l_eachFrameData.cameraPosition		= Camera::GetCamera(g_cameraIndex)->GetPosition();
	l_eachFrameData.inverseProjection	= l_inverseProjection;
	l_eachFrameData.inverseView			= l_inverseView;
	
	*(CustomPrimitiveStruct::EachFrameDataStructure*)mappedResource.pData = l_eachFrameData;
	g_DeviceContext->Unmap(g_EveryFrameBuffer, 0);
}

HRESULT CreatePrimitiveBuffer()
{
	HRESULT hr = S_OK;

	int ByteWidth;

	D3D11_BUFFER_DESC PrimitiveData;
	PrimitiveData.BindFlags			=	D3D11_BIND_CONSTANT_BUFFER;
	PrimitiveData.Usage				=	D3D11_USAGE_DYNAMIC; 
	PrimitiveData.CPUAccessFlags	=	D3D11_CPU_ACCESS_WRITE;
	PrimitiveData.MiscFlags			=	0;
	ByteWidth						=	sizeof(CustomPrimitiveStruct::Primitive);
	PrimitiveData.ByteWidth			=	ByteWidth;
	hr = g_Device->CreateBuffer(&PrimitiveData, NULL, &g_PrimitivesBuffer);

	return hr;
}

HRESULT	CreateDispatchBuffer()
{
	HRESULT hr = S_OK;

	int ByteWidth;

	D3D11_BUFFER_DESC dispatch_buffer_desc;
	dispatch_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	dispatch_buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
	dispatch_buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	dispatch_buffer_desc.MiscFlags = 0;
	ByteWidth = sizeof(OnePerDispatch);
	dispatch_buffer_desc.ByteWidth = ByteWidth;
	hr = g_Device->CreateBuffer(&dispatch_buffer_desc, NULL, &g_dispatchBuffer);
	
	return hr;
}

void UpdateDispatchBuffer(int l_x, int l_y)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	g_DeviceContext->Map(g_dispatchBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	OnePerDispatch g_OnePerDispatch;

	g_OnePerDispatch.client_width = g_resolutionData.width;
	g_OnePerDispatch.client_height = g_resolutionData.height;
	g_OnePerDispatch.x_dispatch_count = l_x;
	g_OnePerDispatch.y_dispatch_count = l_y;

	*(OnePerDispatch*)mappedResource.pData = g_OnePerDispatch;
	g_DeviceContext->Unmap(g_dispatchBuffer, 0);
}

HRESULT	CreateTempBufferAndUAV()
{
	HRESULT hr = S_OK;
	int ByteWidth;
	
	// RAW VERTEX SAVING
	D3D11_BUFFER_DESC temp_buffer_desc;
	temp_buffer_desc.BindFlags				= D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	temp_buffer_desc.Usage					= D3D11_USAGE_DEFAULT;
	temp_buffer_desc.CPUAccessFlags			= 0;
	temp_buffer_desc.MiscFlags				= D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	ByteWidth								= g_resolutionData.width * g_resolutionData.height * 4 *  sizeof(XMFLOAT4);
	temp_buffer_desc.ByteWidth = ByteWidth;
	temp_buffer_desc.StructureByteStride = sizeof(XMFLOAT4);
	hr = g_Device->CreateBuffer(&temp_buffer_desc, NULL, &g_tempBuffer);
	if (FAILED(hr))
		return hr;

	D3D11_UNORDERED_ACCESS_VIEW_DESC temp_buffer_UAV_desc;
	temp_buffer_UAV_desc.Buffer.FirstElement	= 0;
	temp_buffer_UAV_desc.Buffer.Flags			= 0;

	temp_buffer_UAV_desc.Buffer.NumElements		= g_resolutionData.width * g_resolutionData.height * 4;
	temp_buffer_UAV_desc.Format					= DXGI_FORMAT_UNKNOWN;
	temp_buffer_UAV_desc.ViewDimension			= D3D11_UAV_DIMENSION_BUFFER;
	hr = g_Device->CreateUnorderedAccessView(g_tempBuffer, &temp_buffer_UAV_desc, &g_tempUAV);
	if (FAILED(hr))
		return hr;

	return hr;
}


float a = 0.0f;
float b = 0.0f;
bool goOneway = false;
void FillPrimitiveBuffer(float l_deltaTime)
{
	D3D11_MAPPED_SUBRESOURCE PrimitivesResources;
	g_DeviceContext->Map(g_PrimitivesBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &PrimitivesResources);
	CustomPrimitiveStruct::Primitive l_primitive;
		
	l_primitive.Sphere[0].MidPosition			= XMFLOAT4 (0.0f, 500.0f, 700.0f, 1.0f);
	l_primitive.Sphere[0].Radius				= 200.0f;
	l_primitive.Sphere[0].Color					= XMFLOAT3(0.0f, 0.0f, 0.50f);
	
	l_primitive.Sphere[1].MidPosition			= XMFLOAT4 (-900.0f, 500.0f, 700.0f, 1.0f);
	l_primitive.Sphere[1].Radius				= 200.0f;
	l_primitive.Sphere[1].Color					= XMFLOAT3(0.50f, 0.0f, 0.0f);

	l_primitive.Sphere[2].MidPosition			= XMFLOAT4(500.0f, 500.0f, 2200.0f, 0.0f);
	l_primitive.Sphere[2].Radius				= 200.0f;
	l_primitive.Sphere[2].Color					= XMFLOAT3(0.0f, 0.50f, 0.0f);

	l_primitive.Sphere[3].MidPosition			= XMFLOAT4(-1200.0f, 750.0f, 2200.0f, 0.0f);
	l_primitive.Sphere[3].Radius				= 200.0f;
	l_primitive.Sphere[3].Color					= XMFLOAT3(0.0f, 0.0f, 0.0f);

	for(UINT i = 0; i < SPHERE_COUNT; i++)
	{
		float ambient = 0.1f;
		float diffuse = 0.7f;
		float specular = 1.0f;
		l_primitive.Sphere[i].Material.ambient = XMFLOAT3(ambient, ambient, ambient);
		l_primitive.Sphere[i].Material.diffuse = XMFLOAT3(diffuse, diffuse, diffuse);
		l_primitive.Sphere[i].Material.specular = XMFLOAT3(specular, specular, specular);
		l_primitive.Sphere[i].Material.shininess = 50.0f;
		l_primitive.Sphere[i].Material.isReflective = 1.0f;
		l_primitive.Sphere[i].Material.reflectiveFactor = 1.0f;
	}

	if(goOneway)
	{	
		a += 5;
		b = sin(a*PI/1000)*1000;
		if(a > 1000)
		{
			goOneway = false;
		}
	}
	else
	{
		a -= 5;
		b = -sin(a*PI/1000)*1000;
		if(a < 0)
		{
			goOneway = true;
		}
	}
	
	l_primitive.Sphere[0].MidPosition = XMFLOAT4(a, b, 250.0f, 1.0f);

	*(CustomPrimitiveStruct::Primitive*)PrimitivesResources.pData = l_primitive;
	g_DeviceContext->Unmap(g_PrimitivesBuffer, 0);
}

HRESULT CreateLightBuffer()
{
	HRESULT hr = S_OK;

	int ByteWidth;

	D3D11_BUFFER_DESC LightData;
	LightData.BindFlags			=	D3D11_BIND_CONSTANT_BUFFER;
	LightData.Usage				=	D3D11_USAGE_DYNAMIC; 
	LightData.CPUAccessFlags	=	D3D11_CPU_ACCESS_WRITE;
	LightData.MiscFlags			=	0;
	ByteWidth					=	sizeof(CustomLightStruct::LightBuffer);
	LightData.ByteWidth			=	ByteWidth;
	hr = g_Device->CreateBuffer( &LightData, NULL, &g_LightBuffer);

	return hr;
}


float c = 0.0f;
float d = 0.0f;
bool goOneway2 = false;
void FillLightBuffer()
{
	D3D11_MAPPED_SUBRESOURCE LightResources;
	g_DeviceContext->Map(g_LightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &LightResources);

	CustomLightStruct::LightBuffer l_light;
		
	for(UINT i = 0; i < LIGHT_COUNT; i++)
	{
		l_light.pointLight[i].position		= Camera::GetCamera(i)->GetPosition();
		l_light.pointLight[i].color			= XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	}

	
	if(g_cameraIndex != 0)
	{
		if(goOneway)
		{	
			c += 5;
			d = sin(c*PI/1000)*1000;
			if(c > 1000)
			{
				goOneway2 = false;
			}
		}
		else
		{
			c -= 5;
			d = -sin(c*PI/1000)*1000;
			if(c < 0)
			{
				goOneway = true;
			}

		}
		l_light.pointLight[0].position		= XMFLOAT4(c, d, 10.0f, 0.0f);
		Camera::GetCamera(0)->SetPosition(l_light.pointLight[0].position);
	}
	

	*(CustomLightStruct::LightBuffer*)LightResources.pData = l_light;
	g_DeviceContext->Unmap(g_LightBuffer, 0);
}

HRESULT LoadMesh(char* p_path)
{
	// Memory leaks everywhere
	HRESULT hr = S_OK;

	using namespace std;
	vector<XMFLOAT4>* l_rawVertex	= nullptr;
	vector<XMFLOAT2>* l_rawTexCoord = nullptr;
	vector<CustomPrimitiveStruct::TriangleDescription>* l_triangleDescription = nullptr;
	vector<XMFLOAT3>* l_rawNormal = nullptr;

	hr = ObjectLoader::GetObjectLoader()->LoadObject(g_DeviceContext, p_path, &l_rawVertex, &l_rawTexCoord, &l_triangleDescription, &l_rawNormal);
	if(FAILED(hr))
		return hr;

	// Take sizes before updating
	int allVertexSize			= g_allTrianglesVertex		.size();
	int allTexCoordSize			= g_allTrianglesTexCoord	.size();
	int allTrianglesIndexSize	= g_allTrianglesIndex		.size();
	int allTrianglesNormalSize	= g_allTriangleNormal		.size();

	// Move raw vertices
	for(UINT i = 0; i < l_rawVertex->size(); i++)
		g_allTrianglesVertex.push_back(l_rawVertex->at(i));

	// Move raw triangle coords
	for(UINT i = 0; i < l_rawTexCoord->size(); i++)
		g_allTrianglesTexCoord.push_back(l_rawTexCoord->at(i));
	
	// Move Triangle descriptions
	for(UINT i = 0; i < l_triangleDescription->size(); i++)
		g_allTrianglesIndex.push_back(l_triangleDescription->at(i));
	
	// Update triangle descriptions indexes
	if(allVertexSize != 0) // No point to increase everything with 0
	{
		for(UINT i = allTrianglesIndexSize; i < g_allTrianglesIndex.size(); i++)
		{
			g_allTrianglesIndex.at(i).Point1 += allVertexSize;
			g_allTrianglesIndex.at(i).Point2 += allVertexSize;
			g_allTrianglesIndex.at(i).Point3 += allVertexSize;
			g_allTrianglesIndex.at(i).TexCoord1 += allTexCoordSize;
			g_allTrianglesIndex.at(i).TexCoord2 += allTexCoordSize;
			g_allTrianglesIndex.at(i).TexCoord3 += allTexCoordSize;
			g_allTrianglesIndex.at(i).NormalIndex += allTrianglesNormalSize;
		}
	}

	// Move Normal descriptions
	for (UINT i = 0; i < l_rawNormal->size(); i++)
		g_allTriangleNormal.push_back(l_rawNormal->at(i));

	return hr;
}

HRESULT LoadObjectData()
{
	HRESULT hr = S_OK;
	hr = LoadMesh("CUBE.obj");
	if(FAILED(hr))
		return hr;

	return hr; 
}

HRESULT CreateObjectBuffer()
{
	HRESULT hr = S_OK;

	D3D11_SUBRESOURCE_DATA l_data;
	int ByteWidth;
	////////
	// RAW VERTEX SAVING
	l_data.pSysMem = g_allTrianglesVertex.data();
	D3D11_BUFFER_DESC RawVertex;
	RawVertex.BindFlags			=	D3D11_BIND_SHADER_RESOURCE;
	RawVertex.Usage				=	D3D11_USAGE_DEFAULT;
	RawVertex.CPUAccessFlags	=	0;
	RawVertex.MiscFlags			=	D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	ByteWidth					=	g_allTrianglesVertex.size() * sizeof(XMFLOAT4);
	RawVertex.ByteWidth			=	ByteWidth;
	RawVertex.StructureByteStride = sizeof(XMFLOAT4);
	hr = g_Device->CreateBuffer(&RawVertex, &l_data, &g_vertexBuffer);
	if(FAILED(hr))
		return hr;
	D3D11_SHADER_RESOURCE_VIEW_DESC Vertex_SRV_Desc;
	ZeroMemory(&Vertex_SRV_Desc, sizeof(Vertex_SRV_Desc));
	Vertex_SRV_Desc.Buffer.ElementOffset = 0;
	Vertex_SRV_Desc.Buffer.FirstElement = 0;
	Vertex_SRV_Desc.Buffer.NumElements = g_allTrianglesVertex.size();
	Vertex_SRV_Desc.Format = DXGI_FORMAT_UNKNOWN;
	Vertex_SRV_Desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	hr = g_Device->CreateShaderResourceView(g_vertexBuffer, &Vertex_SRV_Desc, &g_Vertex_SRV);
	if (FAILED(hr))
		return hr;

	////////
	// RAW TEXCOORD
	l_data.pSysMem = g_allTrianglesTexCoord.data();
	D3D11_BUFFER_DESC RawTexCoord;
	RawTexCoord.BindFlags			=	D3D11_BIND_SHADER_RESOURCE;	
	RawTexCoord.Usage				=	D3D11_USAGE_DEFAULT; 
	RawTexCoord.CPUAccessFlags		=	0;
	RawTexCoord.MiscFlags			=	D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	ByteWidth						=	g_allTrianglesTexCoord.size() * sizeof(XMFLOAT2); 
	RawTexCoord.ByteWidth			=	ByteWidth;
	RawTexCoord.StructureByteStride =	sizeof(XMFLOAT2);
	hr = g_Device->CreateBuffer( &RawTexCoord, &l_data, &g_TexCoordBuffer);	
	if(FAILED(hr))
		return hr;	
	D3D11_SHADER_RESOURCE_VIEW_DESC TexCoord_SRV_Desc;
	ZeroMemory(&TexCoord_SRV_Desc, sizeof(TexCoord_SRV_Desc));
	TexCoord_SRV_Desc.Buffer.ElementOffset = 0;
	TexCoord_SRV_Desc.Buffer.FirstElement = 0;
	TexCoord_SRV_Desc.Buffer.NumElements = g_allTrianglesTexCoord.size();
	TexCoord_SRV_Desc.Format = DXGI_FORMAT_UNKNOWN;
	TexCoord_SRV_Desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	hr = g_Device->CreateShaderResourceView(g_TexCoordBuffer, &TexCoord_SRV_Desc, &g_TexCoord_SRV);
	if (FAILED(hr))
		return hr;

	////////
	// TriangleDescription
	l_data.pSysMem = g_allTrianglesIndex.data();
	D3D11_BUFFER_DESC ObjectBufferDescription;
	ObjectBufferDescription.BindFlags			=	D3D11_BIND_SHADER_RESOURCE;
	ObjectBufferDescription.Usage				=	D3D11_USAGE_DEFAULT; 
	ObjectBufferDescription.CPUAccessFlags		=	0;
	ObjectBufferDescription.MiscFlags			=	D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	ByteWidth									=	g_allTrianglesIndex.size() * sizeof(CustomPrimitiveStruct::TriangleDescription);
	ObjectBufferDescription.ByteWidth			=	ByteWidth;
	ObjectBufferDescription.StructureByteStride =	sizeof(CustomPrimitiveStruct::TriangleDescription);
	hr = g_Device->CreateBuffer( &ObjectBufferDescription, &l_data, &g_objectBuffer);
	if(FAILED(hr))
		return hr;
	D3D11_SHADER_RESOURCE_VIEW_DESC TrinagleIndex_SRV_Desc;
	ZeroMemory(&TrinagleIndex_SRV_Desc, sizeof(TrinagleIndex_SRV_Desc));
	TrinagleIndex_SRV_Desc.Buffer.ElementOffset = 0;
	TrinagleIndex_SRV_Desc.Buffer.FirstElement = 0;
	TrinagleIndex_SRV_Desc.Buffer.NumElements = g_allTrianglesIndex.size();
	TrinagleIndex_SRV_Desc.Format = DXGI_FORMAT_UNKNOWN;
	TrinagleIndex_SRV_Desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	hr = g_Device->CreateShaderResourceView(g_objectBuffer, &TrinagleIndex_SRV_Desc, &g_TriangleDesc_SRV);
	if(FAILED(hr))
		return hr;

	////////
	// Raw normal desc
	l_data.pSysMem = g_allTriangleNormal.data();
	D3D11_BUFFER_DESC NormalBufferDescription;
	NormalBufferDescription.BindFlags			= D3D11_BIND_SHADER_RESOURCE;
	NormalBufferDescription.Usage				= D3D11_USAGE_DEFAULT;
	NormalBufferDescription.CPUAccessFlags		= 0;
	NormalBufferDescription.MiscFlags			= D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	ByteWidth									= g_allTriangleNormal.size() * sizeof(XMFLOAT3);
	NormalBufferDescription.ByteWidth			= ByteWidth;
	NormalBufferDescription.StructureByteStride = sizeof(XMFLOAT3);
	hr = g_Device->CreateBuffer(&NormalBufferDescription, &l_data, &g_normalBuffer);
	if (FAILED(hr))
		return hr;
	D3D11_SHADER_RESOURCE_VIEW_DESC Normal_SRV_Desc;
	ZeroMemory(&Normal_SRV_Desc, sizeof(Normal_SRV_Desc));
	Normal_SRV_Desc.Buffer.ElementOffset	= 0;
	Normal_SRV_Desc.Buffer.FirstElement		= 0;
	Normal_SRV_Desc.Buffer.NumElements		= g_allTriangleNormal.size();
	Normal_SRV_Desc.Format					= DXGI_FORMAT_UNKNOWN;
	Normal_SRV_Desc.ViewDimension			= D3D11_SRV_DIMENSION_BUFFEREX;
	hr = g_Device->CreateShaderResourceView(g_normalBuffer, &Normal_SRV_Desc, &g_Normal_SRV);
	if (FAILED(hr))
		return hr;
	
	return hr;
}

HRESULT SetSmallBoxTexture()
{
	HRESULT hr = S_OK;

	hr = DirectX::CreateDDSTextureFromFile(g_Device, L"texture/Box_Texture.dds", nullptr, &g_smallBoxTexSRV);

	return hr;
}

HRESULT Update(float deltaTime)
{

	POINT p;
	if (GetCursorPos(&p))
	{
		if(ScreenToClient(g_hWnd, &p))
		{

			POINT l_mousePos;
			int dx,dy;
			l_mousePos.x = p.x;
			l_mousePos.y = p.y;
			dx = p.x - g_resolutionData.width / 2;
			dy = p.y - g_resolutionData.height / 2;


//			Camera::GetCamera(g_cameraIndex)->pitch(	dy * MOUSE_SENSE);
//			Camera::GetCamera(g_cameraIndex)->rotateY(	dx * MOUSE_SENSE);
		
			Camera::GetCamera(g_cameraIndex)->update(dx*MOUSE_SENSE, dy*MOUSE_SENSE);

			p.x = g_resolutionData.width / 2;
			p.y = g_resolutionData.height / 2;
			ClientToScreen(g_hWnd, &p);
			SetCursorPos(p.x,p.y);
			ShowCursor(FALSE);//hides the cursor
		}	
	}



	if(GetAsyncKeyState('W') & 0x8000)
		Camera::GetCamera(g_cameraIndex)->walk(MOVE_SPEED * deltaTime);
	if(GetAsyncKeyState('S') & 0x8000)
		Camera::GetCamera(g_cameraIndex)->walk(MOVE_SPEED* -deltaTime);
	if(GetAsyncKeyState('A') & 0x8000)
		Camera::GetCamera(g_cameraIndex)->strafe(MOVE_SPEED * -deltaTime);
	if(GetAsyncKeyState('D') & 0x8000)
		Camera::GetCamera(g_cameraIndex)->strafe(MOVE_SPEED *deltaTime);
	
	float cameraspeed = 3.0;
//	if(GetAsyncKeyState('Q') & 0x8000)
//		Camera::GetCamera(g_cameraIndex)->rotateY(-cameraspeed * deltaTime);
//	if(GetAsyncKeyState('E') & 0x8000)
//		Camera::GetCamera(g_cameraIndex)->rotateY(cameraspeed * deltaTime);
//	if(GetAsyncKeyState('1') & 0x8000)
//		Camera::GetCamera(g_cameraIndex)->pitch(	-cameraspeed * deltaTime);
//	if(GetAsyncKeyState('2') & 0x8000)
//		Camera::GetCamera(g_cameraIndex)->pitch(	cameraspeed * deltaTime);
	
	float upndownspeed = 18.0f;
	if(GetAsyncKeyState(VK_SPACE) & 0x8000)
		Camera::GetCamera(g_cameraIndex)->MoveY(	MOVE_SPEED * deltaTime);
	if(GetAsyncKeyState(VK_LSHIFT) & 0x8000)
		Camera::GetCamera(g_cameraIndex)->MoveY(	-MOVE_SPEED * deltaTime);
		

	if(GetAsyncKeyState(VK_NUMPAD0) & 0x8000)
			g_cameraIndex = 0;
	if(GetAsyncKeyState(VK_NUMPAD1) & 0x8000)
			g_cameraIndex = 1;
	if(GetAsyncKeyState(VK_NUMPAD2) & 0x8000)
			g_cameraIndex = 2;
	if(GetAsyncKeyState(VK_NUMPAD3) & 0x8000)
			g_cameraIndex = 3;
	if(GetAsyncKeyState(VK_NUMPAD4) & 0x8000)
			g_cameraIndex = 4;
	if(GetAsyncKeyState(VK_NUMPAD5) & 0x8000)
			g_cameraIndex = 5;
	if(GetAsyncKeyState(VK_NUMPAD6) & 0x8000)
			g_cameraIndex = 6;
	if(GetAsyncKeyState(VK_NUMPAD7) & 0x8000)
			g_cameraIndex = 7;
	if(GetAsyncKeyState(VK_NUMPAD8) & 0x8000)
			g_cameraIndex = 8;
	if(GetAsyncKeyState(VK_NUMPAD9) & 0x8000)
			g_cameraIndex = 9;

	

	Camera::GetCamera(g_cameraIndex)->rebuildView();	
	
	FillCameraBuffer();				//
	FillLightBuffer();				//
	FillPrimitiveBuffer(deltaTime);
	
	return S_OK;
}

HRESULT Render(float deltaTime)
{
	ID3D11UnorderedAccessView* uav[] = { g_BackBufferUAV, g_tempUAV };
	ID3D11Buffer* ppCB[] = { g_EveryFrameBuffer, g_PrimitivesBuffer, g_LightBuffer, g_dispatchBuffer};
	ID3D11ShaderResourceView* srv[] = { g_Vertex_SRV, g_TriangleDesc_SRV, g_Normal_SRV, g_TexCoord_SRV, g_smallBoxTexSRV};

	g_DeviceContext->CSSetUnorderedAccessViews(0, 2, uav, 0);
	g_DeviceContext->CSSetConstantBuffers(0, 4, ppCB);
	g_DeviceContext->CSSetShaderResources(0, 5, srv);


	g_Timer->Start();
	for (unsigned int y = 0; y < g_resolutionData.RayTraceCallsY; y++)
	{	
		for (unsigned int x = 0; x < g_resolutionData.RayTraceCallsX; x++)
		{
			RayTracingRender->Set();
			UpdateDispatchBuffer(x, y);
			g_DeviceContext->CSSetConstantBuffers(0, 4, ppCB);		// Send all buffers because I was lazy at the beginning. However, it works now.
																	// Could not get it only to send one buffer, dont remember why.
			g_DeviceContext->Dispatch(g_resolutionData.AmountofThreadGroupWhenRaytracingX, g_resolutionData.AmountofThreadGroupWhenRaytracingY, 1);
			RayTracingRender->Unset();
		}
	}

	// Dont wanna do supersampling with all the 
	if(g_resolutionData.DoSupersamling) {
		SuperSampleRender->Set();
		g_DeviceContext->Dispatch(g_resolutionData.AmountofThreadGroupWhenRenderingX, g_resolutionData.AmountofThreadGroupWhenRenderingY, 1);
		SuperSampleRender->Unset();
	}
	g_Timer->Stop();

	if(FAILED(g_SwapChain->Present(0, 0)))
		return E_FAIL;

	char title[256];
	sprintf_s(
		title,
		sizeof(title),
		"BTH - DirectCompute raytracing - Dispatch time: %f ",
		g_Timer->GetTime());
	SetWindowText(g_hWnd, title);

	return S_OK;
}

//--------------------------------------------------------------------------------------
// Called every time the application receives a message
//--------------------------------------------------------------------------------------
LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	PAINTSTRUCT ps;
	HDC hdc;
	POINT l_mousePos;
	int dx,dy;

	switch (message) 
	{
//	case MK_LBUTTON:
//		g_mouse_clicked = true;
//		GpuPickingBySendingRay(m_oldMousePos.x, m_oldMousePos.y);
//		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		switch(wParam)
		{
			case VK_ESCAPE:
				PostQuitMessage(0);
				break;
		}
		break;
	case WM_MOUSEMOVE:
//		if(wParam & MK_LBUTTON)
//		{
//			l_mousePos.x = (int)LOWORD(lParam);
//			l_mousePos.y = (int)HIWORD(lParam);
			
//			dx = l_mousePos.x - m_oldMousePos.x;
//			dy = l_mousePos.y - m_oldMousePos.y;
		//	Camera::GetCamera(g_cameraIndex)->pitch(	dy * MOUSE_SENSE);
		//	Camera::GetCamera(g_cameraIndex)->rotateY(	-dx * MOUSE_SENSE);
//			m_oldMousePos = l_mousePos;
//		}
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

char* FeatureLevelToString(D3D_FEATURE_LEVEL featureLevel)
{
	if(featureLevel == D3D_FEATURE_LEVEL_11_0)
		return "11.0";
	if(featureLevel == D3D_FEATURE_LEVEL_10_1)
		return "10.1";
	if(featureLevel == D3D_FEATURE_LEVEL_10_0)
		return "10.0";

	return "Unknown";
}

HRESULT SetSampler()
{
	D3D11_SAMPLER_DESC sampler_desc;
	ZeroMemory(&sampler_desc, sizeof(sampler_desc));
	sampler_desc.Filter			= D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampler_desc.AddressU		= D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressV		= D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressW		= D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampler_desc.MinLOD			= 0;
	sampler_desc.MaxLOD			= D3D11_FLOAT32_MAX;

	HRESULT hr = g_Device->CreateSamplerState(&sampler_desc, &g_samplerState);
	if (FAILED(hr))
		return hr;

	g_DeviceContext->CSSetSamplers(0, 1, &g_samplerState);
	return hr;	
}