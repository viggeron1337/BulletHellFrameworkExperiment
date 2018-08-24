#include "Entity.h"

Entity::Entity(std::string textureName, short typeOfInputLayout, DirectX::XMVECTOR position)
{

	ID3DBlob* pVS = nullptr; 

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
	//Core::pDevice->CreateInputLayout(entInput, ARRAYSIZE(entInput), pVS->GetBufferPointer(), 
}

Entity::~Entity()
{
}

void Entity::updateMatrixes()
{
}
