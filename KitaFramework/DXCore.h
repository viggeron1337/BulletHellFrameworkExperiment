#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>
#include "DXUtil.h"
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <d3d11.h>
#include <chrono>
#include <Keyboard.h>

using namespace std::chrono;
using namespace DirectX;

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "runtimeobject.lib")
#pragma comment (lib, "d3dcompiler.lib")

namespace Core
{
    extern ID3D11Device* pDevice;
	extern ID3D11DeviceContext* pImmediateContext;

	extern ID3D11VertexShader* pEntityVS; 
	extern ID3D11GeometryShader* pEntityGS; 
	extern ID3D11PixelShader* pEntityPS;

	extern ID3D11InputLayout* pEntityInputLayout; 

	extern ID3D11SamplerState* pEntitySamplerState; 

	extern ID3D10Blob* VS; 
	extern ID3D10Blob* GS; 
	extern ID3D10Blob* PS;
}

class DXCore
{
public: 
	DXCore(HINSTANCE hInstance); 
	virtual ~DXCore(); 

	//MAIN GAME LOOP
	int Run(); 

	//Framework Methods
	virtual bool Init(); 
	virtual void Update(float dt) = 0; 
	virtual void Render(float dt) = 0; 
	virtual LRESULT msgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam); 

protected: 
	//WIN32 ATTRIBUTES
	HWND m_hGameWnd; 
	HINSTANCE m_hGameInstance; 
	UINT m_clientWidth; 
	UINT m_clientHeight; 
	std::string m_gameTitle; 
	DWORD m_wndStyle; 
	
	struct CAMERA_VALUES
	{
		XMFLOAT3 position; 
		XMFLOAT3 lookAt; 
		XMFLOAT3 up; 
	};

	//Constant Buffer Data Defintion
	struct CONSTANT_BUFFER
	{
		XMFLOAT4X4 wvpMatrix; 
		XMFLOAT4X4 worldMatrix; 
		XMVECTOR lightPos; 
	};

	//DIRECTX ATTRIBUTES
	IDXGISwapChain* m_pSwapChain; 
	ID3D11RenderTargetView* m_pRenderTargetView; 
	D3D_DRIVER_TYPE m_driverType; 
	D3D_FEATURE_LEVEL m_featureLevel; 
	D3D11_VIEWPORT m_viewPort; 
	ID3D11VertexShader* m_pVertexShader; 
	ID3D11PixelShader* m_pPixelShader; 
	ID3D11GeometryShader* m_pGeometryShader; 
	ID3D11InputLayout* m_pvertexLayout; 
	ID3D11Buffer* m_pVertexBuffer; 
	ID3D11Buffer* m_pConstantBuffer; 
	CONSTANT_BUFFER m_constantBuffer; 
	D3D11_BUFFER_DESC m_constantBufferDesc; 
	ID3DBlob* m_ppVS; 
	ID3DBlob* m_pErrorBlob;
	ID3D11DepthStencilView* m_pDepthStencilView; 
	ID3D11Texture2D* m_pDepthStencilBuffer; 
	ID3D11ShaderResourceView* m_pTextureView; 
	ID3D11SamplerState* m_pSamplerState; 

	std::unique_ptr<Keyboard> m_keyboard;

	//Initialize Win32 Window
	bool InitWindow(); 

	//initialize Direct3D 
	bool initDirect3D(); 
	bool initVertexLayouts(); 
	bool initSamplers(); 

	void initKeyboard();

	//Camera Values 
	CAMERA_VALUES m_cameraValues; 

	//Timers 
	float dt;
	std::chrono::high_resolution_clock::time_point timePoint; 
	std::chrono::high_resolution_clock::time_point lastTimePoint; 

	//TEMP
	float rotation; 
	ID3D11Texture2D* m_pQuadTexture; 
	ID3D11SamplerState* m_pQuadTexSampler; 
	ID3D11Resource* textureRes; 
	XMMATRIX m_Mtranslation; 
	XMVECTOR m_currentPos; 
};