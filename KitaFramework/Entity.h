#ifndef ENTITY_H
#define ENTITY_H

#include <string>
#include <direct.h>
#include <DirectXMath.h>
#include <d3d11.h>
#include "DXCore.h"


class Entity
{
private:
	DirectX::XMMATRIX m_worldMatrix; 
	DirectX::XMMATRIX m_viewMatrix; 
	DirectX::XMMATRIX m_projectionMatrix; 
	DirectX::XMMATRIX m_rotationMatrix; 
	DirectX::XMVECTOR m_position; //World
	
	float m_speed; 

	ID3D11Texture2D* m_texture; 
	ID3D11Resource* m_textureResource; 
	
	D3D11_SHADER_RESOURCE_VIEW_DESC m_textureResourceView; 
	D3D11_TEXTURE2D_DESC m_textureDesc; 

	struct ENTITY_STRUCT
	{
		float topLeftX, topLeftY; 
		float sizeX, sizeY; 
		float opacity; 
		float z_order; 
	};

	ID3D11InputLayout* m_vertexInputLayout;

public:
	Entity(std::string textureName, short typeOfInputLayout, DirectX::XMVECTOR position); 
	~Entity(); 

	void updateMatrixes(); 
};

#endif
