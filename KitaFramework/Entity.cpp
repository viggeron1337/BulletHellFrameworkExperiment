#include "Entity.h"

Entity::Entity(std::string textureName, DirectX::XMVECTOR position, float width, float height)
{
	HRESULT result; 

	//Creating texture for the quad that is to be our entity. 
	m_textureDesc.Width = width;
	m_textureDesc.Height = height;
	m_textureDesc.MipLevels = 1;
	m_textureDesc.ArraySize = 1;
	m_textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	m_textureDesc.SampleDesc.Count = 1;
	m_textureDesc.SampleDesc.Quality = 0;
	m_textureDesc.Usage = D3D11_USAGE_DEFAULT;
	m_textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	m_textureDesc.CPUAccessFlags = 0;
	m_textureDesc.MiscFlags = 0;

	result = Core::pDevice->CreateTexture2D(&m_textureDesc, nullptr, &m_pTexture);

	//Shader resource view description for quad texture resource
	D3D11_SHADER_RESOURCE_VIEW_DESC resViewDesc;
	ZeroMemory(&resViewDesc, sizeof(resViewDesc));
	resViewDesc.Format = m_textureDesc.Format;
	resViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	resViewDesc.Texture2D.MipLevels = m_textureDesc.MipLevels;
	resViewDesc.Texture2D.MostDetailedMip = 0;

	//Create the shader resource view and tell it to use the quadTex as a resource. 
	Core::pDevice->CreateShaderResourceView(m_pTexture, &resViewDesc, &m_pTextureResourceView);

	//Cast the texture2D to a reosurce so you can offcially create a texture from a file. 

	std::string fileName = textureName + ".jpg";
	std::wstring widestr = std::wstring(fileName.begin(), fileName.end());
	const wchar_t* widecstr = widestr.c_str();
	
	m_pTextureResource = dynamic_cast<ID3D11Resource*>(m_pTexture);

	//Puts the wished texture onto textureRes / quadTexture loaded from a file. 
	CreateWICTextureFromFile(Core::pDevice, Core::pImmediateContext, widecstr, &m_pTextureResource, &m_pTextureResourceView);
}

Entity::~Entity()
{
}

void Entity::updateMatrixes()
{
}
