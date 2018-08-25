#include "DXCore.h"
#include <wrl.h>
#include <wrl/wrappers/corewrappers.h>

ID3D11Device* Core::pDevice; 
ID3D11DeviceContext* Core::pImmediateContext;
ID3D11InputLayout* Core::pEntityInputLayout;
ID3D11SamplerState* Core::pEntitySamplerState;

ID3D10Blob* Core::VS; 
ID3D10Blob* Core::GS; 
ID3D10Blob* Core::PS; 


namespace
{
	//USED TO FORWARD MESSAGES TO USER DEFINED PROC FUNCTION
	DXCore* g_pCore = nullptr; 
}

LRESULT CALLBACK MainWindProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (g_pCore) return g_pCore->msgProc(hwnd, msg, wParam, lParam);
	else return DefWindowProc(hwnd, msg, wParam, lParam);
}

DXCore::DXCore(HINSTANCE hInstance)
{
	m_hGameInstance = hInstance; 
	m_hGameWnd = NULL; 
	m_clientWidth = 800; 
	m_clientHeight = 600; 
	m_gameTitle = "The Kita Project"; 
	m_wndStyle = WS_OVERLAPPEDWINDOW;
	g_pCore = this; 

	Core::pDevice = nullptr; 
	Core::pImmediateContext = nullptr; 
	m_pRenderTargetView = nullptr; 
	m_pSwapChain = nullptr; 

	m_Mtranslation = XMMatrixTranslation(0, 0, 0); 
	m_currentPos = XMVECTOR{ 0,0,0 }; 
	
}

DXCore::~DXCore()
{
	//Clean up Direct3D
	Memory::safeRelease(m_pVertexBuffer); 
	Memory::safeRelease(m_pvertexLayout);
	Memory::safeRelease(m_pVertexShader);
	Memory::safeRelease(m_pPixelShader); 
	Memory::safeRelease(m_pGeometryShader);
	Memory::safeRelease(m_pConstantBuffer);
	Memory::safeRelease(m_pRenderTargetView);
	Memory::safeRelease(m_pSwapChain);
	Memory::safeRelease(Core::pDevice);
	Memory::safeRelease(Core::pImmediateContext);
	Memory::safeRelease(m_pDepthStencilView);
	Memory::safeRelease(m_pDepthStencilBuffer);
	Memory::safeRelease(m_pTextureView);

	DestroyWindow(m_hGameWnd); 
}

int DXCore::Run()
{
	//Main Messages Loop
	MSG msg = { 0 }; 

	if (m_hGameWnd)
	{
		while (WM_QUIT != msg.message)
		{
			if (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
			{
				TranslateMessage(&msg); 
				DispatchMessage(&msg); 
			}
			else
			{
				//Render
				this->timePoint = std::chrono::high_resolution_clock::now(); 
				this->dt = duration<float>(this->timePoint - this->lastTimePoint).count(); 
				this->lastTimePoint = this->timePoint; 
				Update(dt / 60); 
				Render(dt / 60); 
			}
		}
	}
	return static_cast<int>(msg.wParam); 
}

bool DXCore::Init()
{
	bool initSucceded = true; 
	if (!InitWindow())
	{
		initSucceded = false; 
	}
	if (!initDirect3D())
	{
		initSucceded = false; 
	}

	return initSucceded;
}

bool DXCore::initDirect3D()
{
	UINT createDeviceFlags = 0; 

#ifdef  _DEBUG
	//createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG; 
	createDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#endif //  _DEBUG

	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE
	}; 
	UINT numOfDriverTypes = ARRAYSIZE(driverTypes);

	D3D_FEATURE_LEVEL faetureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3
	}; 
	UINT numOfFeatureLevels = ARRAYSIZE(faetureLevels); 

	DXGI_SWAP_CHAIN_DESC swapDesc; 
	swapDesc.BufferCount = 1; //Double buffered
	swapDesc.BufferDesc.Width = m_clientWidth; 
	swapDesc.BufferDesc.Height = m_clientHeight; 
	swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; 
	swapDesc.BufferDesc.RefreshRate.Numerator = 1; 
	swapDesc.BufferDesc.RefreshRate.Denominator = 60; 
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; 
	swapDesc.OutputWindow = m_hGameWnd; 
	swapDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapDesc.Windowed = true; // temp
	swapDesc.SampleDesc.Count = 1; 
	swapDesc.SampleDesc.Quality = 0; 
	swapDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // alt-enter full screen

	HRESULT result; 
	for (int i = 0; i < numOfDriverTypes; i++)
	{
		result = D3D11CreateDeviceAndSwapChain(NULL, driverTypes[i], NULL, createDeviceFlags,
			faetureLevels, numOfFeatureLevels, D3D11_SDK_VERSION, &swapDesc,
			&m_pSwapChain, &Core::pDevice, &m_featureLevel, &Core::pImmediateContext); 
		if (SUCCEEDED(result))
		{
			m_driverType = driverTypes[i]; 
			break; 
		}
	}

	if (FAILED(result))
	{
		OutputDebugString("FAILED TO CREATE DEVICE AND SWAP CHAIN");
		return false; 
	}

	//Create render target view
	ID3D11Texture2D* pBackBufferTex = 0; 
	m_pSwapChain->GetBuffer(NULL, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackBufferTex)); 
	Core::pDevice->CreateRenderTargetView(pBackBufferTex, nullptr, &m_pRenderTargetView);
	Memory::safeRelease(pBackBufferTex); 

	//Depth Buffer 
	D3D11_TEXTURE2D_DESC textureDesc; 
	textureDesc.Width = m_clientWidth; 
	textureDesc.Height = m_clientHeight;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	Core::pDevice->CreateTexture2D(&textureDesc, nullptr, &m_pDepthStencilBuffer);
	Core::pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, nullptr, &m_pDepthStencilView); 

	//Prepare Texturing 
	Windows::Foundation::Initialize();

	initVertexLayouts(); 
	initSamplers(); 

	//Bind Render Target View
	Core::pImmediateContext->OMSetRenderTargets(1, &m_pRenderTargetView, nullptr); 

	//Viewport Creation 
	m_viewPort.Width = static_cast<float>(m_clientWidth); 
	m_viewPort.Height = static_cast<float>(m_clientHeight);
	m_viewPort.TopLeftX = 0; 
	m_viewPort.TopLeftY = 0; 
	m_viewPort.MinDepth = 0.0f; 
	m_viewPort.MaxDepth = 1.0f; 

	//Bind Viewport
	Core::pImmediateContext->RSSetViewports(1, &m_viewPort); 

	//Creating Vertex Shader
	ID3DBlob* pVS = nullptr;
	ID3DBlob* errorBlob = nullptr;
	D3DCompileFromFile(
		L"vertex.hlsl", // filename
		nullptr,		// optional macros
		nullptr,		// optional include files
		"VS_main",		// entry point 
		"vs_5_0",		// shader model (target)
		D3DCOMPILE_DEBUG,       // shader compile options
		0,				// effect compile options
		&pVS,			// double pointer to ID3DBlob
		&errorBlob		    // pointer to Error Blob messages.
	);

	if (FAILED(result))
	{
		if (errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			OutputDebugStringA("OUCH VERTEX");
			// release "reference" to errorBlob interface object
			errorBlob->Release();
		}
		if (pVS)
			pVS->Release();
		return result;
	}

	HRESULT shaderCreationCheck = Core::pDevice->CreateVertexShader(pVS->GetBufferPointer(), pVS->GetBufferSize(), nullptr, &m_pVertexShader); 

	//Create input layout (verified using vertex shader)
	D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
		
		{ "POSITION",		// "semantic" name in shader
		0,				// "semantic" index (not used)
		DXGI_FORMAT_R32G32B32_FLOAT, //size of ONE element (3 floats)
		0,							 // input slot
		0,							 // offset of first element 
		D3D11_INPUT_PER_VERTEX_DATA, // specify data PER vertex
		0							 // used for INSTANCING (ignore)
		},
		
		{"TEXCOORD",		// "semantic" name in shader
		0,				// "semantic" index (not used)
		DXGI_FORMAT_R32G32_FLOAT, //size of ONE element (2 floats)
		0,						 // input slot
		12,							 // offset of first element 
		D3D11_INPUT_PER_VERTEX_DATA, // specify data PER vertex
		0							 // used for INSTANCING (ignore)
		},
		
		{"COLOR",
		0,							// same slot as previous (same vertexBuffer)
		DXGI_FORMAT_R32G32B32_FLOAT,
		0,
		20,							// offset of FIRST element (after TEXCOORD)
		D3D11_INPUT_PER_VERTEX_DATA,
		0
		},
		
		{"NORMAL",
		0,
		DXGI_FORMAT_R32G32B32_FLOAT,
		0,
		32,
		D3D11_INPUT_PER_VERTEX_DATA,
		0
		}
	};

	HRESULT layoutCreationCheck = Core::pDevice->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), pVS->GetBufferPointer(), pVS->GetBufferSize(), &m_pvertexLayout);
	// We do not need this COM object anymore, so we release it. 
	Memory::safeRelease(pVS);

	//Creating Pixel Shader
	ID3DBlob* pPS;
	if (errorBlob) errorBlob->Release();
	errorBlob = nullptr;
	D3DCompileFromFile(
		L"fragment.hlsl",
		nullptr,
		nullptr,
		"PS_main",
		"ps_5_0",
		D3DCOMPILE_DEBUG,
		0,
		&pPS,
		&errorBlob
	);

	if (FAILED(result))
	{
		if (errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			OutputDebugStringA("OUCH");
			// release "reference" to errorBlob interface object
			errorBlob->Release();
		}
		if (pPS)
			pPS->Release();
		return result;
	}

	shaderCreationCheck = Core::pDevice->CreatePixelShader(pPS->GetBufferPointer(), pPS->GetBufferSize(), nullptr, &m_pPixelShader);
	//We do not need this COM object anymore, so we release it. 
	Memory::safeRelease(pPS);

	//CREATE TRIANGLE DATA
	struct TriangleVertex
	{
		float x, y, z;
		float tX, tY; 
		float r, g, b; 
		float nX, nY, nZ; 
	};

	TriangleVertex triangleVertices[12] =
	{
		-0.5f, 0.5f, 0.0f, //v0
		0.0f, 0.0f, //Tex
		1.0f, 0.0f, 0.0f, // Color
		0.0f, 0.0f, -1.0f, // Normal

		0.5f, 0.5f, 0.0f, //v1
		1.0f, 0.0f, //Tex
		1.0f, 0.0f, 0.0f, // Color
		0.0f, 0.0f, -1.0f, // Normal

		-0.5f, -0.5f, 0.0f, //v2
		0.0f, 1.0f, //Tex
		1.0f, 0.0f, 0.0f, // Color
		0.0f, 0.0f, -1.0f, // Normal

		0.5f, 0.5f, 0.0f, //v3
		1.0f, 0.0f, //Tex
		1.0f, 0.0f, 0.0f, // Color
		0.0f, 0.0f, -1.0f, // Normal

		0.5f, -0.5f, 0.0f, //v4
		1.0f, 1.0f, //Tex
		1.0f, 0.0f, 0.0f, // Color
		0.0f, 0.0f, -1.0f, // Normal

		-0.5f, -0.5f, 0.0f, //v5
		0.0f, 1.0f, //Tex
		1.0f, 0.0f, 0.0f, // Color
		0.0f, 0.0f, -1.0f, // Normal
	};

	//Describe the Vertex Buffer 
	D3D11_BUFFER_DESC bufferDesc;
	memset(&bufferDesc, 0, sizeof(bufferDesc));
	//What type of buffer will this be?
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	//What type of usage?
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	//How big in bytes eac element in the buffer is. 
	bufferDesc.ByteWidth = sizeof(triangleVertices);

	//This struct is created just to set a pointer to the
	//data containing the vertices.
	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = triangleVertices;

	//create a Vertex Buffer. 
	HRESULT bufferCreationCheck = Core::pDevice->CreateBuffer(&bufferDesc, &data, &m_pVertexBuffer);

	m_cameraValues = CAMERA_VALUES{ XMFLOAT3(0.0f,0.0f,-3.0f), XMFLOAT3(0.0f,0.0f,0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) };

	//CREATE GEOMETRY SHADER
	ID3DBlob* pGS = nullptr;
	D3DCompileFromFile(
		L"geometry.hlsl", // filename
		nullptr,		// optional macros
		nullptr,		// optional include files
		"GS_main",		// entry point 
		"gs_5_0",		// shader model (target)
		D3DCOMPILE_DEBUG,       // shader compile options
		0,				// effect compile options
		&pGS,			// double pointer to ID3DBlob
		&errorBlob		    // pointer to Error Blob messages.
	);
	Core::pDevice->CreateGeometryShader(pGS->GetBufferPointer(), pGS->GetBufferSize(), nullptr, &m_pGeometryShader);
	Memory::safeRelease(pGS);

	//Initialize keyboard and bind it to window
	initKeyboard(); 

	//Constant buffer creation (containing camera values).  

	XMVECTOR camPos = XMLoadFloat3(&m_cameraValues.position);
	XMVECTOR lookAt = XMLoadFloat3(&m_cameraValues.lookAt);
	XMVECTOR up = XMLoadFloat3(&m_cameraValues.up);

	XMMATRIX Mproj = XMMatrixPerspectiveFovLH(XMConvertToRadians(45), static_cast<float>(m_clientWidth) / m_clientHeight, 0.1f, 20.0f);
	XMMATRIX Mrotation = XMMatrixRotationY(0.0f);
	XMMATRIX Mview = XMMatrixLookAtLH(camPos, lookAt, up);
	XMMATRIX Mtrans = XMMatrixTranslation(0, 0, 0);
	XMMATRIX Mwvp = XMMatrixIdentity();
	XMMATRIX Mworld = Mrotation * Mtrans;

	//Mworld = Mproj * Mview * Mtranslation * Mrotation; 
	Mwvp = Mworld *  Mview * Mproj;

	//Light position
	m_constantBuffer.lightPos = XMVECTOR({ 0,0,-3 }); 

	//Transposes the world matrix.
	Mwvp = XMMatrixTranspose(Mwvp);
	Mworld = XMMatrixTranspose(Mworld);

	XMVECTOR lookDir = XMLoadFloat3(&m_cameraValues.lookAt); 
	XMVECTOR McamPos = XMLoadFloat3(&m_cameraValues.position); 
	//XMVECTOR lightRay = McamPos - lookDir; 

	//Insert into constant buffer. 
	XMStoreFloat4x4(&m_constantBuffer.wvpMatrix, Mwvp);
	XMStoreFloat4x4(&m_constantBuffer.worldMatrix, Mworld);

	// Fill in a buffer descrption
	m_constantBufferDesc.ByteWidth = sizeof(CONSTANT_BUFFER);
	m_constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	m_constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_constantBufferDesc.MiscFlags = 0;
	m_constantBufferDesc.StructureByteStride = 0;

	// Fill in the subresource data.
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = &m_constantBuffer;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	result = Core::pDevice->CreateBuffer(&m_constantBufferDesc, &InitData, &m_pConstantBuffer);
	if (FAILED(result))
		return false;


	Core::pImmediateContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	//m_pImmediateContext->GSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	Core::pImmediateContext->PSSetConstantBuffers(0, 1, &m_pConstantBuffer);

	Core::pImmediateContext->PSSetSamplers(0, 1, &m_pQuadTexSampler); 
	Core::pImmediateContext->PSSetShaderResources(0, 1, &m_pTextureView); 

	ShowWindow(m_hGameWnd, SW_SHOW);

	return true;
}

bool DXCore::initVertexLayouts()
{
	HRESULT result;

	//Entity input layout
	D3D11_INPUT_ELEMENT_DESC entInput[] = {

		{ "POSITION",		// "semantic" name in shader
		0,				// "semantic" index (not used)
		DXGI_FORMAT_R32G32_FLOAT, //size of ONE element (2 floats)
		0,							 // input slot
		0,							 // offset of first element 
		D3D11_INPUT_PER_VERTEX_DATA, // specify data PER vertex
		0							 // used for INSTANCING (ignore)
		},

		{ "TEXCOORD",		// "semantic" name in shader
		0,				// "semantic" index (not used)
		DXGI_FORMAT_R32G32_FLOAT, //size of ONE element (2 floats)
		0,						 // input slot
		8,							 // offset of first element 
		D3D11_INPUT_PER_VERTEX_DATA, // specify data PER vertex
		0							 // used for INSTANCING (ignore)
		},

		{ "OPACITY",
		0,							// same slot as previous (same vertexBuffer)
		DXGI_FORMAT_R32_FLOAT,
		0,
		16,							// offset of FIRST element (after TEXCOORD)
		D3D11_INPUT_PER_VERTEX_DATA,
		0
		},

		{ "Z-ORDER",
		0,
		DXGI_FORMAT_R32_FLOAT,
		0,
		20,
		D3D11_INPUT_PER_VERTEX_DATA,
		0
		}

	};
	Core::pDevice->CreateInputLayout(entInput, ARRAYSIZE(entInput), Core::VS->GetBufferPointer(), Core::VS->GetBufferSize(), &Core::pEntityInputLayout); 

	return true; 
}

bool DXCore::initSamplers()
{
	HRESULT result; 

	//Sampler for entity textures
	D3D11_SAMPLER_DESC entSampDesc;
	entSampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	entSampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	entSampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	entSampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	entSampDesc.MipLODBias = 0.0f;
	entSampDesc.MaxAnisotropy = 1;
	entSampDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	entSampDesc.BorderColor[0] = 0;
	entSampDesc.BorderColor[1] = 0;
	entSampDesc.BorderColor[2] = 0;
	entSampDesc.BorderColor[3] = 0;
	entSampDesc.MinLOD = 0;

	result = Core::pDevice->CreateSamplerState(&entSampDesc, &Core::pEntitySamplerState);

	return true; 
}

void DXCore::initKeyboard()
{
	m_keyboard = std::make_unique<Keyboard>();
}

LRESULT DXCore::msgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_SIZE:
		m_clientHeight = HIWORD(lParam);
		m_clientWidth = LOWORD(lParam);
		return 0; 

	case WM_ACTIVATEAPP:
		Keyboard::ProcessMessage(msg, wParam, lParam);
		break;

	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
		Keyboard::ProcessMessage(msg, wParam, lParam);
		break;

	default:
		DefWindowProc(hwnd, msg, wParam, lParam);
	}
}

bool DXCore::InitWindow()
{
	//WNDCLASSEX 
	WNDCLASSEX wcex;
	ZeroMemory(&wcex, sizeof(WNDCLASSEX));
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.hInstance = m_hGameInstance;
	wcex.lpfnWndProc = MainWindProc;
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = "DXAPPWNDCLASS";
	wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&wcex))
	{
		OutputDebugString("FAILED TO CREATE WINDOW CLASS\n");
		return false;
	}


	RECT r = { 0, 0, m_clientWidth, m_clientHeight};
	AdjustWindowRect(&r, m_wndStyle, FALSE);
	UINT width = r.right - r.left;
	UINT height = r.bottom - r.top;

	UINT x = GetSystemMetrics(SM_CXSCREEN) / 2 - width / 2;
	UINT y = GetSystemMetrics(SM_CYSCREEN) / 2 - height / 2;


	m_hGameWnd = CreateWindow("DXAPPWNDCLASS", "The Kita Project", m_wndStyle,
		x, y, width, height, NULL, NULL, m_hGameInstance, NULL);

	if (!m_hGameWnd)
	{
		OutputDebugString("FAILED TO CREATE WINDOW\n");
		return false;
	}

	return true;
}



