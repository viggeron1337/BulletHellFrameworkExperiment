#ifndef ENTITY_H
#define ENTITY_H

#include <string>
#include <direct.h>
#include <DirectXMath.h>
#include <d3d11.h>
#include "DXCore.h"
#include "WICTextureLoader.h"


class Entity
{
private:
	DirectX::XMMATRIX m_worldMatrix; 
	DirectX::XMMATRIX m_viewMatrix; 
	DirectX::XMMATRIX m_projectionMatrix; 
	DirectX::XMMATRIX m_rotationMatrix; 
	DirectX::XMVECTOR m_position; //World
	
	float m_speed; 

	ID3D11Texture2D* m_pTexture; 
	ID3D11ShaderResourceView* m_pTextureResourceView; 
	ID3D11Resource* m_pTextureResource; 
	
	D3D11_SHADER_RESOURCE_VIEW_DESC m_textureResourceViewDesc; 
	D3D11_TEXTURE2D_DESC m_textureDesc; 

	struct ENTITY_STRUCT
	{
		float topLeftX, topLeftY; 
		float sizeX, sizeY; 
		float opacity; 
		float z_order; 
	};

public:
	Entity(std::string textureName, DirectX::XMVECTOR position, float width, float height); 
	~Entity(); 

	void updateMatrixes(); 
};

#endif
