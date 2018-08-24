#pragma once
#include "DXCore.h"

class gameFile : public DXCore
{
public: 
	gameFile(HINSTANCE hInstance); 
	~gameFile(); 

	bool Init() override; 
	void Update(float dt) override; 
	void Render(float dt) override; 
	
	void createTexture(); 

};
