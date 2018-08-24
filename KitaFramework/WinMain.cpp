#include <windows.h>
#include "DXCore.h"
#include "gameFile.h"
//#include <d3d11.h>
#pragma comment(lib, "user32.lib")

int CALLBACK WinMain(HINSTANCE hinstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	AllocConsole();
	FILE* fp;
	freopen_s(&fp, "CONOUT$", "w", stdout);

	gameFile tGame(hinstance);

	if (!tGame.Init()) return 1;

	return tGame.Run();
	return 0; 
}