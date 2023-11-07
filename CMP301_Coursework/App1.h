// Application.h
#ifndef _APP1_H
#define _APP1_H

// Includes
#include "DXF.h"	// include dxframework
#include "LightShader.h"
#include "TerrainShader.h"


class App1 : public BaseApplication
{
public:

	App1();
	~App1();
	void init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN);

	bool frame();

protected:
	bool render();
	void gui();

private:
	
	PlaneMesh* terrain;
	PlaneMesh* water;

	TerrainShader* terrainShader;
	LightShader* lightShader;

	Light* light1;
	Light* light2;
	Light* light3;
};

#endif