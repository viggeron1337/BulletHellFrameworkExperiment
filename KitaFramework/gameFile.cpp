#include "gameFile.h"
#include <iostream>
#include <cassert>

gameFile::gameFile(HINSTANCE hInstance) : DXCore(hInstance)
{
}

gameFile::~gameFile()
{
}

bool gameFile::Init()
{
	if (!DXCore::Init())
	{
		return false; 
	}

	this->createTexture(); 
	return true;
}

void gameFile::Update(float dt)
{
	
	auto kb = m_keyboard->GetState(); 

	XMVECTOR offset = XMVECTOR{ 0,0,0 }; 

	if(kb.IsKeyDown(Keyboard::Keys::Left))
	{
		offset = XMVECTOR{ -0.001f,0,0 };
		m_currentPos = XMVectorAdd(m_currentPos, offset); 
	}
	else if (kb.IsKeyDown(Keyboard::Keys::Right))
	{
		offset = XMVECTOR{ 0.001f,0,0 };
		m_currentPos = XMVectorAdd(m_currentPos, offset);
	}
	
	if (kb.IsKeyDown(Keyboard::Keys::Up))
	{
		offset = XMVECTOR{ 0,0.001f,0 };
		m_currentPos = XMVectorAdd(m_currentPos, offset);
	}
	else if (kb.IsKeyDown(Keyboard::Keys::Down))
	{
		offset = XMVECTOR{ 0,-0.001f,0 };
		m_currentPos = XMVectorAdd(m_currentPos, offset);
	}
	m_Mtranslation = XMMatrixTranslationFromVector(m_currentPos); 
}	

void gameFile::Render(float dt)
{
	//Clear ther back buffer to deep blue
	float clearColor[] = { 0, 0, 0, 1 }; 

	//Use ImmideateContext to talk to the API
	Core::pImmediateContext->ClearRenderTargetView(m_pRenderTargetView, DirectX::Colors::Black);
	Core::pImmediateContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	UINT vertexSize = sizeof(float) * 11;
	UINT offset = 0;

	//specify the IA Layout (how is data passed)
	Core::pImmediateContext->IASetInputLayout(m_pvertexLayout);

	//specify which vertex buffer to use next.
	Core::pImmediateContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &vertexSize, &offset);

	//specify the topology to use when drawing
	Core::pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//by specifying NULL or nullptr we are disabling
	//that stage in the pipeline
	Core::pImmediateContext->VSSetShader(m_pVertexShader, nullptr, 0);
	Core::pImmediateContext->HSSetShader(nullptr, nullptr, 0);
	Core::pImmediateContext->DSSetShader(nullptr, nullptr, 0);
	//m_pImmediateContext->GSSetShader(m_pGeometryShader, nullptr, 0);
	Core::pImmediateContext->PSSetShader(m_pPixelShader, nullptr, 0);
	Core::pImmediateContext->PSSetShaderResources(0, 1, &m_pTextureView);

	D3D11_MAPPED_SUBRESOURCE dataPtr;
	Core::pImmediateContext->Map(m_pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &dataPtr);

	XMVECTOR XMPosition = XMLoadFloat3(&m_cameraValues.position);
	XMVECTOR XMLookDir = XMLoadFloat3(&m_cameraValues.lookAt);
	XMVECTOR XMup = XMLoadFloat3(&m_cameraValues.up);

	this->rotation += 0.0005f; 
	XMMATRIX Mproj = XMMatrixPerspectiveFovLH(XMConvertToRadians(45), static_cast<float>(m_clientWidth) / m_clientHeight, 0.1f, 20.0f);
	XMMATRIX Mrotation = XMMatrixRotationY(this->rotation);
	XMMATRIX Mview = XMMatrixLookAtLH(XMPosition, XMLookDir, XMup);
	XMMATRIX Mtrans = m_Mtranslation; 
	XMMATRIX Mwvp = XMMatrixIdentity();
	XMMATRIX Mworld = Mrotation * Mtrans;

	Mwvp = Mworld * Mview * Mproj;

	//Transposes the world  and wvp matrices.
	Mwvp = XMMatrixTranspose(Mwvp);
	Mworld = XMMatrixTranspose(Mworld);

	//Apply changes to worldMatrix in constant buffer. 
	XMStoreFloat4x4(&this->m_constantBuffer.wvpMatrix, Mwvp);
	XMStoreFloat4x4(&this->m_constantBuffer.worldMatrix, Mworld);

	//Copy the data
	memcpy(dataPtr.pData, &m_constantBuffer, sizeof(m_constantBuffer));

	// UnMap constant buffer so that we can use it again in the GPU
	Core::pImmediateContext->Unmap(m_pConstantBuffer, 0);

	// set resource to Vertex Shader and Geometry Shader
	Core::pImmediateContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	//m_pImmediateContext->GSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	Core::pImmediateContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

	//issue a draw call for 6 vertices (similar to OpenGL)
	
	/*assert(m_pImmediateContext != nullptr);
	std::cout << "Immidiate context is null!" << std::endl;*/

	Core::pImmediateContext->Draw(6, 0);

	//change front and back-buffer
	m_pSwapChain->Present(0, 0);

}

void gameFile::createTexture()
{
}
